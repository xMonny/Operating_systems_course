#!/bin/bash

if [ "$#" -ne 0 ]; then
	echo "Don't need arguments to be input"
	exit 1
fi

USER_NAME=$(whoami)

if [ "${USER_NAME}" != "root" ]; then
	echo "User is not root"
	exit 2
fi

while read line
do
	CURRENT_USER=$(echo "${line}" | cut -d ':' -f1)
	HOME_DIR=$(echo "${line}" | cut -d ':' -f6)

	if [ -z "${HOME_DIR}" ]; then
		echo "User ${CURRENT_USER} doesn't have homedir"
		continue
	fi

	sudo -u "${CURRENT_USER}" test -w "${HOME_DIR}"

	if [ "$?" -ne 0 ]; then
		echo "User ${CURRENT_USER} cannot write to its homedir"
	fi
done< <(cat /etc/passwd)

