#!/bin/bash

if [ "$#" -lt 1 -o "$#" -gt 2 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

DIRECTORY="${1}"

if ! [ -d "${DIRECTORY}" ]; then
	echo "Invalid name of a directory"
	exit 2
fi

if [ "$#" -eq 2 -a "${2}" != "-a" ]; then
	echo "Expected '-a' as a second argument"
	exit 3
fi

TEMP_FILE=$(mktemp)

if [ "$#" -eq 2 ]; then
	find "${DIRECTORY}" 2>/dev/null > "${TEMP_FILE}"
else
	find "${DIRECTORY}" ! -name ".*" 2>/dev/null > "${TEMP_FILE}"
fi

while read file
do
	BASE_NAME=$(basename "${file}")

	if [ -f "${file}" ]; then
		SIZE=$(stat "${file}" --printf "%s")

		echo "${BASE_NAME} (${SIZE} bytes)"
	fi

	if [ -d "${file}" ]; then #we can add check for -r 
		ENTRIES=$(find "${file}" -mindepth 1 ! -name ".*" 2>/dev/null | wc -l)

		echo "${BASE_NAME} (${ENTRIES} entries)"
	fi
done< <(cat "${TEMP_FILE}")

rm "${TEMP_FILE}"

