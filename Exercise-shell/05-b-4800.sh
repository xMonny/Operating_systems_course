#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

FILE="${1}"
DIRECTORY="${2}"

if ! [ -r "${FILE}" ]; then
	echo "File ${FILE} cannot be read"
	exit 2
fi

if ! [ -d "${DIRECTORY}" ]; then
	echo "Invalid name of directory"
	exit 3
fi

while read name_file
do
	if ! [ -r "${name_file}" ]; then
		continue;
	fi

	CHECK=$(diff -q "${FILE}" "${name_file}")
	
	if [ -z "${CHECK}" ]; then
		echo "${name_file}"
	fi	
done< <(find "${DIRECTORY}" -type f)

#while read -d $'\0' file
#do
#...
#...
#done< <(find "${DIRECTORY}" -type f -print0)
