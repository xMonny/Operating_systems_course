#!/bin/bash

if [ "$#" -ne 0 ]; then
	echo "No arguments needed"
	exit 1
fi

USER_NAME=$(whoami)
if [ "${USER_NAME}" != "root" ]; then
	echo "User is not root"
	exit 2
fi

while read line
do
	CURRENT_USER=$(echo "${line}" \
	    	       | cut -d ':' -f1)
	HOME_DIR=$(echo "${line}" | cut -d ':' -f6)
	if [ -z "${HOME_DIR}" ]; then
		echo "User ${CURRENT_USER} has not home directory"
		continue
	fi

	if [ ! -d "${HOME_DIR}" ]; then
		echo "Directory ${HOME_DIR} is not valid"
		continue
	fi

	OWNER=$(stat "${HOME_DIR}" --printf "%U" 2>/dev/null)
	if [ "${CURRENT_USER}" != "${OWNER}" ]; then
		echo "User ${CURRENT_USER} is not the owner of homedir ${HOME_DIR}"
		continue
	fi

	CAN_WRITE=$(stat "${HOME_DIR}" --printf "%A" 2>/dev/null \
		    | cut -c 3)
	if [ "${CAN_WRITE}" != "w" ]; then
		echo "User ${CURRENT_USER} cannot write to its own home directory"
	fi
done< <(cat /etc/passwd)
