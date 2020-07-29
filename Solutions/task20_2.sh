#!/bin/bash

if [ "$#" -ne 3 ]; then
	echo "You should input source directory, empty destination directory and a string"
	exit 1
fi

if [ ! -d "${1}" ]; then
	echo "Invalid name of source directory"
	exit 1
fi

if [ ! -d "${2}" ]; then
	echo "Invalid name of destination directory"
	exit 1
else
	NUMBER=$(find "${2}" -mindepth 1 | wc -l)
	if [ "${NUMBER}" -ne 0 ]; then
		echo "You have to input an empty destination directory"
		exit 1
	fi
fi

if [ -z "${3}" ]; then
	echo "You should input a string"
	exit 1
fi

SOURCE="${1}"
DESTINATION="${2}"
STRING="${3}"

if [ "$(whoami)" = "monny" ]; then
	while read INSIDE
	do
		echo "${INSIDE}" | fgrep -q "${STRING}"
		if [ $? -eq 0 ]; then
			#get all directory path
			DIR=$(dirname "${INSIDE}")

			#remove parent folder because in the task it is removed
		        REMOVE_BASEDIR=$(dirname "${INSIDE}" | sed "s/${SOURCE}//1")
			
			#get only the name of the file without the path
			FILE=$(basename "${INSIDE}")

			#make directory path in Destination
			mkdir -p "${DESTINATION}"/"${REMOVE_BASEDIR}"
			
			#move the found file in the new folder with same path
			mv "${DIR}"/"${FILE}" "${DESTINATION}"/"${REMOVE_BASEDIR}"
		fi
	done< <(find "${SOURCE}" -mindepth 1 -type f 2>/dev/null)
fi
