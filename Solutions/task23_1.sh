#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "You should input a directory name and a string"
	exit 1
fi

if [ ! -d "${1}" ]; then
	echo "Invalid name of a directory"
	exit 1
fi

DIRECTORY="${1}"
STRING="${2}"

MAXIMUM_X=0
MAXIMUM_Y=0
MAXIMUM_Z=0

while read FILE
do
	BASENAME=$(basename "${FILE}")
	echo "${BASENAME}" | grep -q "^vmlinuz-.*[\.].*[\.].*-${STRING}$"

	if [ $? -eq 0 ]; then
		FILE_XYZ=$(echo "${FILE}" | awk -F '-' '{ print $2 }')
		FILE_X=$(echo "${FILE_XYZ}" | awk -F '.' '{ print $1 }')
		FILE_Y=$(echo "${FILE_XYZ}" | awk -F '.' ' { print $2 }')
		FILE_Z=$(echo "${FILE_XYZ}" | awk -F '.' '{ print $3 }')
		 
		if [ "${FILE_X}" -ge "${MAXIMUM_X}" ]; then
			if [ "${FILE_X}" -gt "${MAXIMUM_X}" ]; then
				 MAXIMUM_X="${FILE_X}"
                                 MAXIMUM_Y="${FILE_Y}"
                                 MAXIMUM_Z="${FILE_Z}"
                                 REMEMBER="${FILE}"
			elif [ "${FILE_Y}" -gt "${MAXIMUM_Y}" ]; then
			 	MAXIMUM_X="${FILE_X}"
                      	 	MAXIMUM_Y="${FILE_Y}"
                       	 	MAXIMUM_Z="${FILE_Z}"
                          	REMEMBER="${FILE}"
 			elif [ "${FILE_Y}" -eq "${MAXIMUM_Y}" ]; then            				if [ "${FILE_Z}" -ge "${MAXIMUM_Z}" ]; then
					MAXIMUM_X="${FILE_X}"
                                 	MAXIMUM_Y="${FILE_Y}"
                                	MAXIMUM_Z="${FILE_Z}"
                                 	REMEMBER="${FILE}"
				fi
			fi
		fi
	fi
done< <(find "${DIRECTORY}" -mindepth 1 -maxdepth 1 -type f)

echo "Maximum version: ${REMEMBER}"
