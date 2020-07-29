#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

FILE="${1}"

if [ ! -f "${FILE}" ]; then
	echo "First argument should be a text file"
	exit 2
fi

if [ ! -r "${FILE}" ]; then
	echo "Input text file doesn't have read permission"
	exit 3
fi

DIRECTORY="${2}"

if [ ! -d "${DIRECTORY}" ]; then
	echo "Second argument should be an empty directory"
	exit 4
fi

if [ ! -w "${DIRECTORY}" ]; then
	echo "Input directory doesn't have write permission"
	exit 5
fi

NUM_FILES=$(find "${DIRECTORY}" -mindepth 1 2>/dev/null | wc -l)

if [ "${NUM_FILES}" -ne 0 ]; then
	echo "Input directory should be empty"
	exit 6
fi

NEW_FILE=$(echo "${DIRECTORY}/dict.txt")
touch "${NEW_FILE}"

NUMBER=1

while read line
do
	FULL_NAME=$(echo "${line}" | cut -d ':' -f1 | cut -d '(' -f1 | sed 's/ $//')
	TEXT=$(echo "${line}" | cut -d ':' -f2 | sed 's/^ *//g')
	WAS_BEFORE=$(cat "${NEW_FILE}" | fgrep "${FULL_NAME}")
	if [ -z "${WAS_BEFORE}" ]; then
		echo "${FULL_NAME};${NUMBER}" >> "${NEW_FILE}"
		DIR_NUMBER=$(echo "${DIRECTORY}/${NUMBER}.txt")
		touch "${DIR_NUMBER}"
		NUMBER=$((NUMBER+1))
	else
		TAKE_NUMBER=$(echo "${WAS_BEFORE}" | cut -d ';' -f2)
		DIR_NUMBER=$(echo "${DIRECTORY}/${TAKE_NUMBER}.txt")
	fi
	echo "${TEXT}" >> "${DIR_NUMBER}"
done< <(cat "${FILE}" | egrep "[a-zA-Z-]+[ ]+[a-zA-Z-]+ *(\([a-zA-Z ]+\))*:[ ]*[^\n]")
