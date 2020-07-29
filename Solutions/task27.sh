#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

FILE_ONE="${1}"

if [ ! -f "${FILE_ONE}" ]; then
	echo "First argument should be a name of csv file"
	exit 2
fi

if [ ! -r "${FILE_ONE}" ]; then
	echo "First input file cannot be read"
	exit 3
fi

FILE_TWO="${2}"

if [ -f "${FILE_TWO}" ]; then
	rm "${FILE_TWO}"
fi

DIR_NAME=$(dirname "${FILE_ONE}")
NEW_FILE=$(echo "${DIR_NAME}/${FILE_TWO}")
touch "${NEW_FILE}"

while read line
do
	HAS_FILES=$(cat "${NEW_FILE}" | wc -l)
	if [ "${HAS_FILES}" -eq 0 ]; then
		echo "${line}" >> "${NEW_FILE}"
		continue
	fi

	GET_ID=$(echo "${line}" | cut -d ',' -f1)
	GET_WITHOUT_ID=$(echo "${line}" | cut -d ',' -f2-)
	
	IS_REPEAT=$(cat "${NEW_FILE}" | fgrep "${GET_WITHOUT_ID}")
	if [ -z "${IS_REPEAT}" ]; then
		echo "${line}" >> "${NEW_FILE}"
		continue
	fi

	GET_SECOND_ID=$(echo "${IS_REPEAT}" | cut -d ',' -f1)
	GET_SECOND_WITHOUT_ID=$(echo "${IS_REPEAT}" | cut -d ',' -f2-)

	if [ "${GET_WITHOUT_ID}" != "${GET_SECOND_WITHOUT_ID}" ]; then
		echo "${line}" >> "${NEW_FILE}"
		continue
	fi

	if [ "${GET_ID}" -lt "${GET_SECOND_ID}" ]; then
		sed -i "s/^${GET_SECOND_ID},/${GET_ID},/1" "${NEW_FILE}"
	fi
done< <(cat "${FILE_ONE}")
