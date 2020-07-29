#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Excpect 1 argument"
	exit 1
fi

DIRECTORY="${1}"

if ! [ -d "${DIRECTORY}" ]; then
	echo "Invalid name of directory"
	exit 2
fi

TEMP_FILE=$(mktemp)

find "${DIRECTORY}" -maxdepth 1 ! -empty -type f -exec md5sum {} + | tr -s ' ' | sort -d | uniq -w32 | cut -d ' ' -f2 > "${TEMP_FILE}"

COUNT=0

while read inDirFile
do
	while read inTempFile
	do
		if [ "${inDirFile}" == "${inTempFile}" ]; then
			COUNT=$((COUNT+1))
			break
		fi
	done< <(cat "${TEMP_FILE}")

	if [ "${COUNT}" -eq 1 ]; then
		COUNT=0
		#echo "SAVE: ${inDirFile}"
		continue
	fi

	echo "DELETE: ${inDirFile}"
	rm "${inDirFile}"

done< <(find "${DIRECTORY}" -maxdepth 1 ! -empty -type f)

rm "${TEMP_FILE}"
