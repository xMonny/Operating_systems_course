#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Expect only 1 argument"
	exit 1
fi

NUMBER="${1}"

if ! [ "${NUMBER}" -eq "${NUMBER}" ] 2>/dev/null; then
	echo "Input argument is not an integer"
	exit 2
fi

who | grep -q "^root "

if [ "$?" -ne 0 ]; then
	echo "User is not root"
	exit 3
fi

START_USERNAME=""
MAX_RSS=0
ALL_RSS=0

while read line
do
	CURRENT_USERNAME=$(echo "${line}" | cut -d ' ' -f1)
	CURRENT_PID=$(echo "${line}" | cut -d ' ' -f3)

	if [ "${START_USERNAME}" = "" ]; then
		START_USERNAME="${CURRENT_USERNAME}"
	elif [ "${START_USERNAME}" != "${CURRENT_USERNAME}" ]; then
		echo "${START_USERNAME} ${ALL_RSS}"
		
		if [ "${ALL_RSS}" -gt "${NUMBER}" ]; then
			echo "KILL: ${CURRENT_PID} ${MAX_RSS}"
			kill -s SIGTERM "${CURRENT_PID}"
			sleep 2
			kill -s SIGKILL "${CURRENT_PID}"
		fi

		START_USERNAME="${CURRENT_USERNAME}"
		ALL_RSS=0
		MAX_RSS=0
	fi

	CURRENT_RSS=$(echo "${line}" | cut -d ' ' -f2)
	ALL_RSS=$((ALL_RSS+CURRENT_RSS))

	if [ "${CURRENT_RSS}" -gt "${MAX_RSS}" ]; then
		MAX_RSS="${CURRENT_RSS}"
	fi
done< <(ps -e -o user=,rss=,pid= --no-header | sort | tr -s ' ')

echo "${CURRENT_USERNAME} ${ALL_RSS}"
if [ "${ALL_RSS}" -gt "${NUMBER}" ]; then
	echo "KILL: ${CURRENT_PID} ${MAX_RSS}"
	kill -s SIGTERM "${CURRENT_PID}"
	sleep 2
	kill -s SIGKILL "${CURRENT_PID}"
fi
