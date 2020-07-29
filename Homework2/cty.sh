#!/bin/bash

if [ "$#" -le 1 -o "$#" -gt 4 ]; then
	echo "You have to input a name of csv file; the words country, zones or distance; a sign/two signs in case distance"
	exit 1
fi

if [ "${2}" = "country" -o "${2}" = "zones" ] && [ "$#" -ne 3 ]; then
	echo "You have to input a name of csv file; the words country or zones; a sign"
	exit 2

elif [ "${2}" = "distance" -a "$#" -ne 4 ]; then
	echo "You have to input a name of csv file; the word distance; two signs"
       	exit 3
fi

if [ ! -f "${1}" ]; then
        echo "Invalid name of file"
        exit 4
fi

FILE=$(mktemp)
COMMAND="${2}"
SIGN="${3}"

BEEN=0

#bonus1
printenv | egrep -q "^CTY_FORMAT=DAT$"

if [ $? -eq 0 ]; then
	BEEN=$((BEEN+1))
	cat "${1}" \
	| tr -s ' ' \
	| sed 's/,/ /g' \
	| sed 's/: /,/g' \
	| sed 's/^ //g' \
	| tr -d '\r\n' \
	| sed 's/:/ /g' \
	| sed 's/;/;\n/g' \
	| sed 's/^/2,&/g' \
	| sed 's/^/1,&/g' \
	| awk -F ',' '{ print $1","$3","$2","$6","$4","$5","$7","$8","$9","$10 }' >> "${FILE}"

fi

if [ "${BEEN}" -eq 0 ]; then
	cat "${1}" >> "${FILE}"
fi

echo "${SIGN}" | egrep -q "^[A-Z0-9]+(/*[A-Z0-9]+)*$"

if [ $? -ne 0 ]; then
        echo "Invalid sign in third argument"
        exit 5
fi

if [ "${COMMAND}" = "distance" -a "$#" -eq 4 ]; then
        SECOND_SIGN="${4}"

        echo "${SECOND_SIGN}" | egrep -q "^[A-Z0-9]+(/*[A-Z0-9]+)*$"

        if [ $? -ne 0 ]; then
                echo "Invalid sign in fourth argument"
                exit 6
        fi
fi

function findSign
{
	INPUT_SIGN="${1}"

	cat "${FILE}" \
	| cut -d ',' -f10 \
	| sed 's/^/",",&/g' \
	| egrep -q "=${INPUT_SIGN}(\([0-9]+\))?(\[[0-9]+\])?( |;)"

	if [ $? -eq 0 ]; then
		FOUND_SIGN=$(cat "${FILE}" \
			   | egrep -o "=${INPUT_SIGN}(\([0-9]+\))?(\[[0-9]+\])?( |;)")

		echo "${FOUND_SIGN}"
		return
	fi

	SAVE=$(mktemp)

	WORD=""

	while read letter
	do 
		WORD=$(echo "${WORD}${letter}")
		cat "${FILE}" \
		| cut -d ',' -f10 \
		| sed 's/^/",",&/g' \
		| egrep -o "( |,)${WORD}(\([0-9]+\))?(\[[0-9]+\])?( |;)" \
		| sed 's/,//g' \
		| sed 's/ //g' \
		| sed -E "s/\(.*//g" \
		| sed -E "s/\[.*//g" \
		| sed 's/;//g' >> "${SAVE}"

	done< <(echo "${INPUT_SIGN}" | grep -o ".")

	LINES=$(cat "${SAVE}" | wc -l)

	if [ "${LINES}" -eq 0 ]; then
		rm "${SAVE}"
		return
	fi

	FOUND_SIGN=$(cat "${SAVE}" \
		     | awk '{ print length, $0 }' \
		     | sort -r \
		     | head -n 1 \
		     | cut -d ' ' -f2)

	FINAL_SIGN=$(cat "${FILE}" \
		     | egrep -o "( |,)${FOUND_SIGN}(\([0-9]+\))?(\[[0-9]+\])?( |;)")

	echo "${FINAL_SIGN}"

	rm "${SAVE}"
}

function convertToRadians
{
	DEGREE="${1}"
	RESULT=$(awk 'BEGIN{ print '${DEGREE}'*3.1415926/180}')
	echo "${RESULT}"
}

function getDistance
{
	LATITUDE_ONE="${1}"
	LONGITUDE_ONE="${2}"
	LATITUDE_TWO="${3}"
        LONGITUDE_TWO="${4}"

	if [ "${LATITUDE_ONE}" = "${LATITUDE_TWO}" -a "${LONGITUDE_ONE}" = "${LONGITUDE_TWO}" ]; then
		echo '0'
		return
	fi

	DIFFERENCE_LATITUDE=$(bc <<< "${LATITUDE_TWO}-(${LATITUDE_ONE})")
	DIFFERENCE_LONGITUDE=$(bc <<< "${LONGITUDE_TWO}-(${LONGITUDE_ONE})")
	
	RADIANS_DIFFERENCE_LATITUDE=$(convertToRadians "${DIFFERENCE_LATITUDE}")
	RADIANS_DIFFERENCE_LONGITUDE=$(convertToRadians "${DIFFERENCE_LONGITUDE}")

	DLAT=$(awk 'BEGIN{ print '${RADIANS_DIFFERENCE_LATITUDE}'/2 }')
	DLON=$(awk 'BEGIN{ print '${RADIANS_DIFFERENCE_LONGITUDE}'/2 }')

	RADIANS_LATITUDE_ONE=$(convertToRadians "${LATITUDE_ONE}")
	RADIANS_LATITUDE_TWO=$(convertToRadians "${LATITUDE_TWO}")

	MATH_ONE=$(awk 'BEGIN{ print (sin('${DLAT}'))^2+cos('${RADIANS_LATITUDE_ONE}')*cos('${RADIANS_LATITUDE_TWO}')*(sin('${DLON}'))^2 }')

	MATH_TWO=$(awk 'BEGIN{ print 2*atan2(sqrt('${MATH_ONE}'),sqrt(1-('${MATH_ONE}'))) }')

	RADIUS=6371

	FINAL=$(awk 'BEGIN{ print '${RADIUS}'*'${MATH_TWO}' }')

	awk 'BEGIN{ printf "%3.0f\n", '${FINAL}' }'
}

function checkHomeCountry
{
        INPUT_SIGN="${1}"

        echo "${INPUT_SIGN}" | grep -q "/"

        if [ $? -eq 0 ]; then
                TAKEN=$(echo "${INPUT_SIGN}" \
                        | cut -d '/' -f2)

                FOUND_SIGN=$(findSign "${TAKEN}")

                if [ -z "${FOUND_SIGN}" ]; then
                        return
                fi

                HOME_COUNTRY=$(cat "${FILE}" \
                	       | fgrep "${FOUND_SIGN}" \
                	       | cut -d ',' -f2)

		echo "${HOME_COUNTRY}"
        fi
}

#a and bonus2)
function findCountry
{
	FOUND_SIGN=$(findSign "${SIGN}")

	if [ -z "${FOUND_SIGN}" ]; then
		rm "${FILE}"
                exit 0
        fi

	HOME_COUNTRY=$(checkHomeCountry "${SIGN}")
	
	COUNTRY=$(cat "${FILE}" \
		  | fgrep "${FOUND_SIGN}" \
		  | cut -d ',' -f2)

	if [ -z "${HOME_COUNTRY}" ]; then
		echo "${COUNTRY}"
	else
		echo "${COUNTRY} (${HOME_COUNTRY})"
	fi
}

#b)
function findZones
{
        FOUND_SIGN=$(findSign "${SIGN}")

	if [ -z "${FOUND_SIGN}" ]; then
		rm "${FILE}"
		exit 0
	fi

	#check if found sign has own ITU zone 
	echo "${FOUND_SIGN}" | egrep -q "\["

	if [ $? -eq 0 ]; then
		ITU=$(echo "${FOUND_SIGN}" | cut -d '[' -f2 | cut -d ']' -f1)
	else
		ITU=$(cat "${FILE}" \
                      | fgrep "${FOUND_SIGN}" \
                      | cut -d ',' -f6)
	fi

	#check if found sign has WAZ zone
	echo "${FOUND_SIGN}" | egrep -q "\("

        if [ $? -eq 0 ]; then
		WAZ=$(echo "${FOUND_SIGN}" | cut -d '(' -f2 | cut -d ')' -f1)
        else
		WAZ=$(cat "${FILE}" \
                      | fgrep "${FOUND_SIGN}" \
                      | cut -d ',' -f5)
	fi

	echo "${ITU} ${WAZ}"
}

#c
function findDistance
{
	FOUND_SIGN_ONE=$(findSign "${SIGN}")
	FOUND_SIGN_TWO=$(findSign "${SECOND_SIGN}")

	if [ -z "${FOUND_SIGN_ONE}" -o -z "${FOUND_SIGN_TWO}" ]; then
		echo "One of the signs are missing from the file"
		rm "${FILE}"
		exit 10
	fi       

	LATITUDE_ONE=$(cat "${FILE}" \
		    | fgrep "${FOUND_SIGN_ONE}" \
		    | cut -d ',' -f7)

	LONGITUDE_ONE=$(cat "${FILE}" \
		      | fgrep "${FOUND_SIGN_ONE}" \
		      | cut -d ',' -f8)

	LATITUDE_TWO=$(cat "${FILE}" \
                    | fgrep "${FOUND_SIGN_TWO}" \
                    | cut -d ',' -f7)            

        LONGITUDE_TWO=$(cat "${FILE}" \
                      | fgrep "${FOUND_SIGN_TWO}" \
                      | cut -d ',' -f8)

	RESULT=$(getDistance "${LATITUDE_ONE}" "${LONGITUDE_ONE}" "${LATITUDE_TWO}" "${LONGITUDE_TWO}")

	echo "${RESULT}"
}

case "${COMMAND}" in
	"country")
		  findCountry
		  rm "${FILE}"
		  exit 0
		  ;;
	  "zones")
		  findZones
		  rm "${FILE}"
	          exit 0
		  ;;
       "distance")
	 	  findDistance
		  rm "${FILE}"
                  exit 0
		  ;;
		*)
		  echo "Invalid command"
		  rm "${FILE}"
		  exit 11
esac
