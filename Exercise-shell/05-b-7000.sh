#!/bin/bash

if [ "$#" -eq 0 ]; then
	echo "Input atleast one argument"
	exit 1
fi

NUM_ARGUMENTS="$#"

read -p "Input a string: " STRING

if [ -z "${STRING}" ]; then #in this case we count the empty lines
	STRING="^$"
fi

while [ "${NUM_ARGUMENTS}" -ne 0 ]
do
	if ! [ -r "${1}" ]; then
		echo "File \"${1}\" cannot be read"
	else
		ROWS=$(cat "${1}" | grep "${STRING}" | wc -l)
		echo "File \"${1}\" has this string in ${ROWS} lines"
	fi
	shift 1
	NUM_ARGUMENTS=$((NUM_ARGUMENTS-1))
done
