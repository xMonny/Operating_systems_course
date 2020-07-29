#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

SOURCE="${1}"
DESTINATION="${2}"

if ! [ -d "${SOURCE}" ]; then
	echo "Invalid name of source directory"
	exit 2
fi

if ! [ -d "${DESTINATION}" ]; then
	echo "Invalid name of destination directory"
	exit 3
fi

if ! [ -r "${SOURCE}" ]; then
	echo "Source directory doesn't have read permissions"
	exit 4
fi

if ! [ -r "${DESTINATION}" ]; then
	echo "Destination directory doesn't have read permissions"
	exit 5
fi

if ! [ -w "${DESTINATION}" ]; then
	echo "Destination directory doesn't have write permissions"
	exit 6
fi

while read -d $'\0' file
do
	BASE_NAME=$(basename "${file}")
	EXTENSION=$(echo "${BASE_NAME}" | egrep "^[^\.]+\.[^\.]+$" | cut -d '.' -f2)
	find "${DESTINATION}" -mindepth 1 -maxdepth 1 -type d 2>/dev/null | grep -q "^${EXTENSION}$"

	if [ "$?" -ne 0 ]; then
		mkdir "${DESTINATION}"/"${EXTENSION}" 2>/dev/null
	fi

	cp "${file}" "${DESTINATION}"/"${EXTENSION}" 2>/dev/null
done< <(find "${SOURCE}" -type f -print0 2>/dev/null)
