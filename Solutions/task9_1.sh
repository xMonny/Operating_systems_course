#!/bin/bash

if [ "$#" -ne 0 ]; then
	echo "No arguments needed"
	exit 1
fi

USER_NAME="pesho"
HOME_DIR=$(cat /etc/passwd | grep "^${USER_NAME}:" | cut -d ':' -f6)
if [ ! -d "${HOME_DIR}" ]; then
	echo "User ${USER_NAME} has invalid home directory"
	exit 2
fi

if [ ! -r "${HOME_DIR}" ]; then
	echo "Home directory ${HOME_DIR} cannot be read"
	exit 3
fi

LAST_MODIFIED=$(find "${HOME_DIR}" -type f -printf "%T@ %p\n" 2>/dev/null | sort -n | tail -n 1 | cut -d ' ' -f2)

INODE=$(stat "${LAST_MODIFIED}" --printf "%i")

echo "${LAST_MODIFIED} ${INODE}"
