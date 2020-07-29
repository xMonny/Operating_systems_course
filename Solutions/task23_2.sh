#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

DIRECTORY="${1}"

if ! [ -d "${DIRECTORY}" ]; then
	echo "First argument should be a name of directory"
	exit 1
fi

if ! [ -r "${DIRECTORY}" ]; then
	echo "Input directory doesn't have read permission"
	exit 2
fi

STRING="${2}"

if [ -z "${STRING}" ]; then
	echo "Second argument should be a non-empty string"
	exit 3
fi

REMEMBER=""

while read line
do
	BASE_NAME=$(basename "${line}")
	if [ "${REMEMBER}" = "" ]; then
		REMEMBER="${BASE_NAME}"
		continue
	fi

	VERSION_LINE=$(echo "${BASE_NAME}" | cut -d '-' -f2)
	X_LINE=$(echo "${VERSION_LINE}" | cut -d '.' -f1)
	Y_LINE=$(echo "${VERSION_LINE}" | cut -d '.' -f2)
	Z_LINE=$(echo "${VERSION_LINE}" | cut -d '.' -f3)

	VERSION_REMEMBER=$(echo "${REMEMBER}" | cut -d '-' -f2)
	X_REMEMBER=$(echo "${VERSION_REMEMBER}" | cut -d '.' -f1)
	Y_REMEMBER=$(echo "${VERSION_REMEMBER}" | cut -d '.' -f2)
	Z_REMEMBER=$(echo "${VERSION_REMEMBER}" | cut -d '.' -f3)

	if [ "${X_LINE}" -gt "${X_REMEMBER}" ]; then
		REMEMBER="${BASE_NAME}"
		continue
	elif [ "${X_LINE}" -eq "${X_REMEMBER}" ]; then
		if [ "${Y_LINE}" -gt "${Y_REMEMBER}" ]; then
			REMEMBER="${BASE_NAME}"
			continue
		elif [ "${Y_LINE}" -eq "${Y_REMEMBER}" ]; then
			if [ "${Z_LINE}" -gt "${Z_REMEMBER}" ]; then
				REMEMBER="${BASE_NAME}"
				continue
			fi
		fi
	fi 
done< <(find "${DIRECTORY}" -mindepth 1 -maxdepth 1 -type f 2>/dev/null | egrep "vmlinuz-[0-9]+\.[0-9]+\.[0-9]+-${STRING}$")

if [ -n "${REMEMBER}" ]; then
	echo "${REMEMBER}"
fi
