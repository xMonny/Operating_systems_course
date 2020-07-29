#!/bin/bash

if [ "$#" -eq 0 ]; then
	echo "Input atleast 1 argument"
	exit 1
fi

ALL="$#"
NUM_ARGUMENTS="$#"
SIGNAL=0
BACKUP=$BACKUP_DIR

while [ "${NUM_ARGUMENTS}" -ne 0 ]
do
	if [ "${NUM_ARGUMENTS}" -eq "${ALL}" -a "${1}" = "-r" ]; then
		SIGNAL=1
		NUM_ARGUMENTS=$((NUM_ARGUMENTS-1))
		shift 1
	fi

	BASE_NAME=$(basename "${1}" 2>/dev/null)
	DATE=$(date +'%F-%H-%M-%S')
	NEW_NAME=$(echo "${BASE_NAME}_${DATE}")

	if [ -f "${1}" ]; then
		tar -czf "${NEW_NAME}".gz "${1}"
		mv "${NEW_NAME}".gz "${BACKUP}"
		rm "${1}"
	elif [ -d "${1}" ]; then
		FILES_INSIDE=$(find "${1}" -mindepth 1 2>/dev/null | wc -l)

		if [ "${FILES_INSIDE}" -ne 0 -a "${SIGNAL}" -eq 0 ]; then
			echo "Error: Directory ${1} cannot be removed"
		else
			tar -czf "${NEW_NAME}".tgz "${1}"
			mv "${NEW_NAME}".tgz "${BACKUP}"
			rm -r "${1}"
		fi
	else
		echo "Error: ${1} is invalid name of a file/directory"
	fi
	
	NUM_ARGUMENTS=$((NUM_ARGUMENTS-1))
	shift 1
done
