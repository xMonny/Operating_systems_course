#!/bin/bash

if [ "$#" -lt 1 -o "$#" -gt 2 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

DIRECTORY="${1}"

if ! [ -d "${DIRECTORY}" ]; then
	echo "First argument should be a valid directory"
	exit 2
fi

if [ ! -r "${DIRECTORY}" -o ! -x "${DIRECTORY}" ]; then
	echo "Directory doesn't have permissions"
	exit 3
fi

if [ "$#" -eq 2 ]; then
	NUMBER="${2}"

	if ! [ "${NUMBER}" -eq "${NUMBER}" ] 2>/dev/null; then
		echo "Only integer can be accepted as second argument"
		exit 4
	fi

	if [ "${NUMBER}" -lt 0 ]; then
		echo "Number cannot be negative"
		exit 5
	fi

	echo "Files in directory ${DIRECTORY} atleast with ${NUMBER} hardlinks:"

	find "${DIRECTORY}" -type f -links "${NUMBER}" 2>/dev/null
	find "${DIRECTORY}" -type f -links +"${NUMBER}" 2>/dev/null
else
	echo "Broken symlinks in directory ${DIRECTORY}: "
	find "${DIRECTORY}" -xtype l 2>/dev/null
fi
