#!/bin/bash

if [ "$#" -ne 0 ]; then
	echo "No arguments needed"
	exit 1
fi

USER_NAME=$(whoami)

HOME_DIR=$(cat /etc/passwd | grep "^${USER_NAME}:" | cut -d ':' -f6)

if [ ! -d "${HOME_DIR}" ]; then
	echo "Homedir ${HOME_DIR} doesn't exist"
	exit 2
fi

while read line
do
	BASE_NAME=$(basename "${line}")
	echo "${BASE_NAME}"
done< <(find "${HOME_DIR}" -type f -printf "%s %p\n" 2>/dev/null | sort -nr | head -n 5 | cut -d ' ' -f2)

