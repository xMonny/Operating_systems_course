#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "You have to input a name of file"
	exit 1
fi

C_FILE="${1}"

if [ ! -r "${C_FILE}" ]; then
	echo "File "${C_FILE}" cannot be read"
	exit 2
fi

#LEVEL=0;
#MAX_LEVEL=0;

#while read line
#do
#	echo "${line}" | grep -q "}"
#	if [ "$?" -eq 0 ]; then
#		LEVEL=$((LEVEL-1))
#	fi

#	echo "${line}" | grep -q "{"
#	if [ "$?" -eq 0 ]; then
#		LEVEL=$((LEVEL+1))
#	fi

#	if [ "${LEVEL}" -gt "${MAX_LEVEL}" ]; then
#		MAX_LEVEL="${LEVEL}"
#	fi
#done< <(cat "${C_FILE}") //read full lines

#echo "The deepest nesting is ${MAX_LEVEL} levels"

LEVEL=0
MAX_LEVEL=0
while read i
do
	if [ "${i}" = "{" ]; then
		LEVEL=$((LEVEL+1))
	fi

	if [ "${LEVEL}" -gt "${MAX_LEVEL}" ]; then
		MAX_LEVEL="${LEVEL}"
	fi

	if [ "${i}" = "}" ]; then
		LEVEL=$((LEVEL-1))
	fi
done< <(cat "${C_FILE}" | egrep -o [{}]) #read only braces

echo "The deepest nesting is ${MAX_LEVEL} levels"
