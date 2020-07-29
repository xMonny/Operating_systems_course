#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Expect only username as first argument"
	exit 1
fi

USER_NAME="${1}"

CHECK_USER=$(id -n -u "${USER_NAME}" 2>/dev/null)
if [ -z "${CHECK_USER}" ]; then
	echo "User ${USER_NAME} is unknown"
	exit 2
fi

#CHECK_USER=$(cat /etc/passwd | grep "^${USER_NAME}:") valid too

CURRENT_USER=$(whoami)

if [ "${CURRENT_USER}" != "root" ]; then
	echo "User is not root"
	exit 3
fi

ALL_PROCESSES_FOR_USERS=$(ps -e -o user= | sort | uniq -c | tr -s ' ')
USER_NUMBER_PROCESSES=$(echo "${ALL_PROCESSES_FOR_USERS}" | grep " ${USER_NAME}"$ | cut -d ' ' -f2)

echo "Users who have more processes than user ${USER_NAME}: "

while read line
do
	CURRENT=$(echo "${line}" | cut -d ' ' -f2)
	if [ "${CURRENT}" = "${USER_NAME}" ]; then
		continue
	fi

	CURRENT_NUMBER_PROCESSES=$(echo "${line}" | cut -d ' ' -f1)
	if [ "${CURRENT_NUMBER_PROCESSES}" -gt "${USER_NUMBER_PROCESSES}" ]; then
		echo "${CURRENT}"
	fi
done< <(echo "${ALL_PROCESSES_FOR_USERS}")

echo -n "Average time in seconds for all processes: "

AVERAGE_SECONDS=$(ps -eo time= | awk -F ':' '{ lines++; sum+=3600*$1 + 60*$2 + $3 } END { print sum/lines }')

echo "${AVERAGE_SECONDS}"

echo -n "Average time in hours for all processes: "
date -d@"${AVERAGE_SECONDS}" -u +%H:%M:%S

echo "Processes of user ${USER_NAME} with double time: "

while read line
do
	CURRENT_PID=$(echo "${line}" | cut -d ' ' -f2)
	CURRENT_TIME=$(echo "${line}" | cut -d ' ' -f3 | awk -F ':' ' { print 3600*$1 + 60*$2 + $3 }')
	DOUBLE_TIMES=$(echo "${CURRENT_TIME}" | awk '{ print $1*2 }')
	COMPARE=$(echo "${DOUBLE_TIMES} ${AVERAGE_SECONDS}" | awk '{ if($1 > $2) print 0; if($1 <= $2) print 1 }')

	if [ "${COMPARE}" = "0" ]; then
		echo "Killing process with ID ${CURRENT_PID}"
		kill -s SIGTERM "${CURRENT_PID}"
		sleep 2
		kill -s SIGKILL "${CURRENT_PID}"
	fi
done< <(ps -eo user=,pid=,time= | tr -s ' ' | grep "^${USER_NAME} ")
