#!/bin/bash

if [ "$#" -ne 3 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

SOURCE="${1}"
DESTINATION="${2}"
STRING="${3}"

if ! [ -d "${SOURCE}" ]; then
	echo "First argument should be a name of source directory"
	exit 1
fi

#if ! [ -r "${SOURCE}" ]; then
#	echo "Source directory doesn't have read permissions"
#	exit 2
#fi

SOURCE_EMPTY=$(find "${SOURCE}" -mindepth 1 | wc -l)
if [ "${SOURCE_EMPTY}" -eq 0 ]; then
	echo "Source directory should not be empty"
	exit 2
fi

if ! [ -d "${DESTINATION}" ]; then
	echo "Second argument should be a name of destination directory"
	exit 3
fi

if [ ! -w "${DESTINATION}" -o ! -x "${DESTINATION}" ]; then
	echo "Destination directory doesn't have necessary permissions"
	exit 4
fi

EMPTY_DESTINATION=$(find "${DESTINATION}" -mindepth 1 | wc -l)
if [ "${EMPTY_DESTINATION}" -ne 0 ]; then
	echo "Destination directory should be empty"
	exit 5
fi

if [ -z "${STRING}" ]; then
	echo "Third argument should be a non-empty string"
	exit 6
fi

SRC=$(dirname "${SOURCE}")"/"$(basename "${SOURCE}")
DST=$(dirname "${DESTINATION}")"/"$(basename "${DESTINATION}")

for i in $(find "${SRC}" -type f -name "*${STRING}*"); do
	DSTFILE=$(echo $i | sed -e "s%${SRC}%${DST}%")
	mkdir -p $(dirname "${DSTFILE}")
	mv $i "${DSTFILE}"
done
