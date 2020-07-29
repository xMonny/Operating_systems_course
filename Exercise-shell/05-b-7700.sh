#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

DIRECTORY="${1}"
NUMBER="${2}"

if ! [ -d "${DIRECTORY}" ]; then
	echo "First argument should be a directory"
	exit 2
fi

if ! [ -r "${DIRECTORY}" ]; then
	echo "Directory ${DIRECTORY} cannot be read"
	exit 3
fi

if ! [ "${NUMBER}" -eq "${NUMBER}" ] 2>/dev/null; then
	echo "Second argument should be a number"
	exit 4
fi

SUM=0

while read -d $'\0' file
do
	SIZE_FILE=$(stat "${file}" --printf "%s")
	if [ "${SIZE_FILE}" -gt "${NUMBER}" ]; then
		SUM=$((SUM+SIZE_FILE))
	fi
done< <(find "${DIRECTORY}" -mindepth 1 -maxdepth 1 -type f -print0 2>/dev/null)

echo "SUM: ${SUM} (bytes)"
