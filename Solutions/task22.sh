#!/bin/bash

if [ "$#" -lt 1 -o "$#" -gt 2 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

DIRECTORY="${1}"

if ! [ -d "${DIRECTORY}" ]; then
	echo "First argument should be a directory"
	exit 2
fi

if ! [ -r "${DIRECTORY}" ]; then
	echo "Input directory doesn't have read permission"
	exit 3
fi

FILE=""

if [ "$#" -eq 2 ]; then
	FILE="${2}"
	if ! [ -f "${FILE}" ]; then
		echo "Second argument should be a file"
		exit 4
	fi

	if ! [ -w "${FILE}" ]; then
		echo "Input file doesn't have write permission"
		exit 5
	fi
fi

BROKEN_SYMLINKS=$(find "${DIRECTORY}" -xtype l 2>/dev/null)
NUM_BROKEN_SYMLINKS=$(find "${DIRECTORY}" -xtype l 2>/dev/null | wc -l)

while read line
do
	IS_BROKEN=$(echo "${BROKEN_SYMLINKS}" | grep "^${line}$")
	if [ -n "${IS_BROKEN}" ]; then
		continue
	fi

	BASE_FILE=$(basename "${line}")
	BASE_DIR=$(dirname "${line}")
	DESTINATION_FILE=$(stat "${line}" | grep " File: " | cut -d ' ' -f4- | cut -d ' ' -f3)
	NEW_NAME=$(echo "${BASE_DIR}/${DESTINATION_FILE}")

	if [ "${FILE}" = "" ]; then
		echo "${BASE_FILE} -> ${NEW_NAME}"
	else
		echo "${BASE_FILE} -> ${NEW_NAME}" >> "${FILE}"
	fi
done< <(find "${DIRECTORY}" -type l 2>/dev/null)

if [ "${FILE}" = "" ]; then
	echo "Broken symlinks: ${NUM_BROKEN_SYMLINKS}"
else
	echo "Broken symlinks: ${NUM_BROKEN_SYMLINKS}" >> "${FILE}"
fi
