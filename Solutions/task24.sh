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

USERS=$(ps -eo user= | sort | uniq | grep -v "^root$")
TEMP_FILE=$(mktemp)

while read CURRENT_USER
do
	USER_HOMEDIR=$(cat /etc/passwd | grep "^${CURRENT_USER}:" | cut -d ':' -f6)
	OWNER_HOMEDIR=$(stat "${USER_HOMEDIR}" --printf "%U" 2>/dev/null)
	HAS_OWNER_WRITE_PERMISSION=$(stat "${USER_HOMEDIR}" --printf "%A" 2>/dev/null | head -c 3 | tail -c 1)
	if ! [ -d "${USER_HOMEDIR}" ]; then
		echo "${CURRENT_USER}" >> "${TEMP_FILE}"
	elif [ "${CURRENT_USER}" != "${OWNER_HOMEDIR}" ]; then
		echo "${CURRENT_USER}" >> "${TEMP_FILE}"
	elif [ "${HAS_OWNER_WRITE_PERMISSION}" != "w" ]; then
		echo "${CURRENT_USER}" >> "${TEMP_FILE}"
	fi	
done< <(echo "${USERS}")

cat "${TEMP_FILE}"

ALL_FOR_ROOT=$(ps -eo user=,rss= | tr -s ' ' | grep "^root " | awk -F ' ' '{ a[$1]+=$2 } END { for(i in a) print i, a[i]}' | cut -d ' ' -f2)

while read CURRENT_USER
do
	ALL_FOR_CURRENT_USER=$(ps -eo user=,rss= | tr -s ' ' | grep "^${CURRENT_USER} " | awk -F ' ' '{ a[$1]+=$2 } END { for(i in a) print i, a[i] }' | cut -d ' ' -f2)
	if [ -z "${ALL_FOR_CURRENT_USER}" ]; then
		continue
	fi
	if [ "${ALL_FOR_CURRENT_USER}" -gt "${ALL_FOR_ROOT}" ]; then
		while read line
		do
			CURRENT_PID=$(echo "${line}" | cut -d ' ' -f2)
			echo "${CURRENT_USER}: KILL ${CURRENT_PID}"
			kill -s SIGTERM "${CURRENT_PID}"
			sleep 2
			kill -s SIGKILL "${CURRENT_PID}"
		done< <(ps -eo user=,pid= | tr -s ' ' | grep "^${CURRENT_USER} ")
	fi
done< <(cat "${TEMP_FILE}")

rm "${TEMP_FILE}"
