#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "You have to input a name of a directory and a number"
	exit 1
fi

DIRECTORY="${1}"
NUMBER="${2}"

if ! [ -d "${DIRECTORY}" ]; then
	echo "First argument should be a name of a directory"
	exit 2
fi

if ! [ -r "${DIRECTORY}" ]; then
	echo "Directory doesn't have read permissions"
	exit 3
fi

if ! [ "${NUMBER}" -eq "${NUMBER}" ] 2>/dev/null; then
	echo "Second argument should be a number"
	exit 4
fi

while read file
do
	BASE_NAME=$(basename "${file}")
	SIZE_FILE=$(stat "${file}" --printf "%s")
	if [ "${SIZE_FILE}" -gt "${NUMBER}" ]; then
		echo "${BASE_NAME} (${SIZE_FILE} bytes)"
	fi
done< <(find "${DIRECTORY}" -mindepth 1 -maxdepth 1 -type f)
