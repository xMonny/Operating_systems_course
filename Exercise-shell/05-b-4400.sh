#!/bin/bash

if [ "$#" -lt 1 -o "$#" -gt 2 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

SOURCE="${1}"

if [ ! -d "${SOURCE}" ]; then
	echo "Invalid name of source directory"
	exit 2;
fi

if [ ! -r "${SOURCE}" ]; then
	echo "Source directory doesn't have read permissions"
	exit 3
fi

if [ "$#" -eq 2 ]; then
	DESTINATION="${2}"
else
	DESTINATION="$(date +"%d-%m-%Y-%r" | tr ':' '-')"
	mkdir "${DESTINATION}"
fi

if [ ! -d "${DESTINATION}" ]; then
	echo "Invalid name of destination directory"
	exit 4;
fi

if [ ! -w "${DESTINATION}" ]; then
	echo "Destination directory doesn't have write permissions"
	exit 5
fi

while read file
do
	cp "${file}" "${DESTINATION}"
done< <(find "${SOURCE}" -type f -mmin -45)

#find "${SOURCE}" -type f -mmin -45 -exec cp {} "${DESTINATION}" \; //valid

tar -cvJf "${DESTINATION}".tar.xz "${DESTINATION}"
