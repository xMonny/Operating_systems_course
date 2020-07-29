#!/bin/bash

if [ "$#" -ne 3 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

SRC="${1}"
DEST="${2}"
STRING="${3}"

if [ ! -d "${SRC}" ]; then
	echo "First argument should be a source directory"
	exit 2
fi

if [ ! -r "${SRC}" -o ! -x "${SRC}" ]; then
	echo "First input directory doesn't have necessary permissions"
	exit 3
fi

if [ ! -d "${DEST}" ]; then
	echo "Second argument should be an empty destination directory"
	exit 4
fi

if [ ! -w "${DEST}" -o ! -x "${DEST}" ]; then
	echo "Second input directory does not have necessary permissions"
	exit 5
fi

IS_EMPTY=$(find "${DEST}" -mindepth 1 2>/dev/null \
	    | wc -l)
if [ "${IS_EMPTY}" -ne 0 ]; then
	echo "Destination directory should be empty"
	exit 6
fi

if [ -z "${STRING}" ]; then
	echo "Third argument should be a non-empty string"
	exit 7
fi

USER_NAME=$(whoami)
if [ "${USER_NAME}" != "monny" ]; then
	echo "User is not root"
	exit 8
fi

while read line
do
	BASE_NAME=$(basename "${line}")
	DIR_NAME=$(dirname "${line}")

	if [ "${DIR_NAME}" = "${SRC}" ]; then
		mv "${line}" "${DEST}"
	else
		NEW_DIR=$(echo "${DIR_NAME}" \
			  | sed "s/${SRC}/${DEST}/1")
		mkdir -p "${NEW_DIR}"
		mv "${line}" "${NEW_DIR}"
	fi
done< <(find "${SRC}" -type f 2>/dev/null | fgrep "${STRING}")
