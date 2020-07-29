#!/bin/bash

if [ "$#" -ne 0 ]; then
	echo "No arguments needed"
	exit 1
fi

while read line
do
	BASE_NAME=$(basename "${line}")
	echo "${BASE_NAME}"
done< <(find . -type f -printf "%n %p\n" 2>/dev/null | sort -nr | head -n 5 | cut -d ' ' -f2)
