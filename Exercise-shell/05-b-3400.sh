#!/bin/bash

read -p "Input a name of file and a string: " FILE STRING

if [ ! -r "${FILE}" ]; then
	echo "File ${FILE} cannot be read"
	exit 10
fi

if [ -z "${STRING}" ]; then
	echo "Empty string"
	exit 11
fi

fgrep -q "${STRING}" "${FILE}"
#cat "${FILE}" | grep -q "${STRING}" //valid too

echo "$?"
