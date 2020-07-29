#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Expect only a name of user"
	exit 1
fi

USER_NAME="${1}"

cat /etc/passwd | grep -q "^${USER_NAME}:"

if [ "$?" -ne 0 ]; then
	echo "User ${USER_NAME} is not in the system"
	exit 2
fi

NUMBER_PROCESSES=0

while read line
do
	PROCESS_PID=$(echo "${line}" | cut -d ' ' -f2)
	echo "Stop process ${PROCESS_PID}"
	kill -s SIGTERM "${PROCESS_PID}"
	sleep 2
	kill -s SIGKILL "${PROCESS_PID}"

	NUMBER_PROCESSES=$((NUMBER_PROCESSES+1))
done< <(ps -e -o user=,pid= | grep "^${USER_NAME} " | tr -s ' ')

echo "Killed processes of user ${USER_NAME}: ${NUMBER_PROCESSES}"
