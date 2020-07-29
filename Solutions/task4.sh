#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

FILE_ONE="${1}"
FILE_TWO="${2}"

if [ ! -f "${FILE_ONE}" ]; then
	echo "First argument should be a file"
	exit 2
fi

if [ ! -r "${FILE_ONE}" ]; then
	echo "First input file cannot be read"
	exit 3
fi

if [ ! -f "${FILE_TWO}" ]; then
	echo "Second argument should be a file"
	exit 4
fi

if [ ! -r "${FILE_TWO}" ]; then
	echo "Second input file cannot be read"
	exit 5
fi

NUMBER_FILE_ONE=$(cat "${FILE_ONE}" | egrep "\. ${FILE_ONE} - " | wc -l)
NUMBER_FILE_TWO=$(cat "${FILE_TWO}" | egrep "\. "${FILE_TWO}" - " | wc -l)

if [ "${NUMBER_FILE_ONE}" -gt "${NUMBER_FILE_TWO}" ]; then
	WINNER="${FILE_ONE}"
else
	WINNER="${FILE_TWO}"
fi

CREATED_FILE=$(echo "${WINNER}.songs")

touch "${CREATED_FILE}"

cat "${WINNER}" | cut -d ' ' -f4- | sort > "${CREATED_FILE}"
