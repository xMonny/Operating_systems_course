#!/bin/bash

if [ "$#" -ne 0 ]; then
	echo "No arguments need"
	exit 1
fi

USER_NAME=$(whoami)

if [ "${USER_NAME}" != "root" ]; then
	echo "User is not root"
	exit 2
fi

USERS_NUMPROCESSES=$(ps -eo user= | sort | uniq -c | tr -s ' ' | awk -F ' ' '{ print $2, $1 }')

USERS_RSS=$(ps -eo user=,rss= | sort | tr -s ' ' | awk -F ' ' '{ a[$1]+=$2 } END { for (i in a) print i, a[i] }' | sort)

USERS_NUMPROCESSES_RSS=$(join <(echo "${USERS_NUMPROCESSES}") <(echo "${USERS_RSS}"))

echo "Usernames, number of active processes, rss: "
echo "${USERS_NUMPROCESSES_RSS}"

#b
AVERAGE_RSS_FOR_EVERY_USER=$(ps -eo user=,rss= | sort | tr -s ' ' | awk -F ' ' '{ a[$1]+=$2; count[$1]+=1 } END { for (i in a) print i, a[i]/count[i] }' | sort)

START_USER=""
MAX_RSS=0

echo

while read line
do
	CURRENT_USER=$(echo "${line}" | cut -d ' ' -f1)
	CURRENT_PID=$(echo "${line}" | cut -d ' ' -f2)
	CURRENT_RSS=$(echo "${line}" | cut -d ' ' -f3)

	if [ "${START_USER}" = "" ]; then
		START_USER="${CURRENT_USER}"
	elif [ "${START_USER}" != "${CURRENT_USER}" ]; then
		DOUBLE_MAX_RSS=$(("${MAX_RSS}"*2))
		TAKE_AVERAGE_RSS=$(echo "${AVERAGE_RSS_FOR_EVERY_USER}" |\
				   grep "^${START_USER} " | cut -d ' ' -f2 |\
				   awk '{ print int($1) }')
		if [ "${TAKE_AVERAGE_RSS}" -eq "${DOUBLE_MAX_RSS}" ]; then
			echo "USER ${START_USER}, KILL ${REMEMBER_PID}"
			kill -s SIGTERM "${REMEMBER_PID}"
			sleep 2
			kill -s SIGKILL "${REMEMBER_PID}"
		fi

		MAX_RSS=0
		START_USER=""	
	fi

	if [ "${CURRENT_RSS}" -gt "${MAX_RSS}" ]; then
		MAX_RSS="${CURRENT_RSS}"
		REMEMBER_PID="${CURRENT_PID}"
	fi
done< <(ps -eo user=,pid=,rss= | sort | tr -s ' ')

DOUBLE_MAX_RSS=$(("${MAX_RSS}"*2))
TAKE_AVERAGE_RSS=$(echo "${AVERAGE_RSS_FOR_EVERY_USER}" |\
		   grep "^${CURRENT_USER} " | cut -d ' ' -f2)
if [ "${TAKE_AVERAGE_RSS}" -eq "${DOUBLE_MAX_RSS}" ]; then
	echo "USER ${CURRENT_USER}, KILL ${REMEMBER_PID}"
	kill -s SIGTERM "${REMEMBER_PID}"
	sleep 2
	kill -s SIGKILL "${REMEMBER_PID}"
fi
