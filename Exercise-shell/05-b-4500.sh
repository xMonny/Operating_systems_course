#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Expect only 1 argument"
	exit 1
fi

USER_NAME="$(id -n -u ${1} 2>/dev/null)"
if [ -z "${USER_NAME}" ]; then
	echo "No such user found"
	exit 2
fi

while [ "$(who | wc -l)" -ne 0 ]
do
	LOGGED="$(who | grep "^${USER_NAME} ")"
	if [ -z "${LOGGED}" ]; then
		echo "Waiting user ${USER_NAME} to log in..."
		sleep 1
	else
		echo "User ${USER_NAME} is logged"
		exit 0
	fi
done
