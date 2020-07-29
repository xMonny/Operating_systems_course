#!/bin/bash

if [ "$#" -eq 0 ]; then
	echo "Input atleast 1 argument"
	exit 1
fi

NUM_ARGUMENTS="$#"

while [ "${NUM_ARGUMENTS}" -ne 0 ]
do
	if ! [ -f "${1}" -o -d "${1}" ]; then
		echo "Invalid name of file/directory"
	fi

	if [ -r "${1}" ]; then
		echo "${1} can be read"
	else
		echo "${1} cannot be read"
	fi

	if [ -d "${1}" ]; then
		NUM_FILES=$(find "${1}" -mindepth 1 -maxdepth 1 -type f 2>/dev/null| wc -l)
		while read inside
		do
			SIZE_INSIDE=$(stat "${inside}" --printf "%s")

			if [ "${SIZE_INSIDE}" -lt "${NUM_FILES}" ]; then
				echo "${inside} (${SIZE_INSIDE} bytes)"
			fi
		done< <(find "${1}" -mindepth 1 -maxdepth 1 -type f 2>/dev/null)
	fi
	NUM_ARGUMENTS=$((NUM_ARGUMENTS-1))
	shift 1
done
