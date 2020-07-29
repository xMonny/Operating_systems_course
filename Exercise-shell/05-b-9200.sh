#!/bin/bash

if [ "$#" -eq 0 ]; then
	echo "Input atleast one argument"
	exit 1
elif [ "$#" -eq 1 -a "${1}" = "-r" ]; then
	echo "Input atleast one argument after '-r'"
	exit 2
fi

ALL="$#"
NUM_ARGUMENTS="$#"

SIGNAL=0

LOG_FILE=$RMLOG_FILE

while [ "${NUM_ARGUMENTS}" -ne 0 ]
do
	if [ "${NUM_ARGUMENTS}" -eq "${ALL}" -a "${1}" = "-r" ]; then
		SIGNAL=$((SIGNAL+1))
		NUM_ARGUMENTS=$((NUM_ARGUMENTS-1))
		shift 1
		continue
	fi

	DATE=$(date +'%F %T')
 
	if [ -f "${1}" ]; then
		echo "[${DATE}] Removed file ${1}" >> "${LOG_FILE}"
		rm "${1}"
	elif [ -d "${1}" ]; then
		FILES_INSIDE=$(find "${1}" -mindepth 1 2>/dev/null | wc -l)
		if [ "${FILES_INSIDE}" -eq 0 ]; then
			echo "[${DATE}] Removed empty directory ${1}" >> "${LOG_FILE}"
			rm -r "${1}"
		elif [ "${SIGNAL}" -eq 1 ]; then
			echo "[${DATE}] Removed directory recursively ${1}" >> "${LOG_FILE}"
			rm -r "${1}"
		else
			echo "Error: Directory ${1} could not be deleted"
		fi
	elif [ ! -f "${1}" -o ! -d "${1}" ]; then
		echo "Error: ${1} is invalid name of file/directory" 
	fi

	NUM_ARGUMENTS=$((NUM_ARGUMENTS-1))
	shift 1
done
