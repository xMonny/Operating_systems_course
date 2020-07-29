#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "Expected name of file and nickname"
	exit 1
fi

FILE="${1}"
NICKNAME="${2}"

if [ ! -r "${FILE}" ]; then
	echo "File "${FILE}" cannot be read"
	exit 2
fi

if [ -z "${NICKNAME}" ]; then
	echo "Empty nickname is input"
	exit 3
fi

REAL_NAME=$(cat "${FILE}" | egrep "^${NICKNAME} " | cut -d ' ' -f2)
if [ -z "${REAL_NAME}" ]; then
	echo "Nickname ${NICKNAME} is not found in file ${FILE}"
	exit 4
fi

cat /etc/passwd | grep -q "^${REAL_NAME}:"

if [ "$?" -ne 0 ]; then
	echo "Username ${REAL_NAME} is not found in the system"
	exit 5
fi

write "${REAL_NAME}"
