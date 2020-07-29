#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Expect only a name of user"
	exit 1
fi

USER_NAME="${1}"

cat /etc/passwd | grep -q "^${USER_NAME}:"

if [ "$?" -ne 0 ]; then
	echo "User ${USER_NAME} doesn't exist in the system"
	exit 2
fi

while read process
do
	NUMBER_PID=$(echo "${process}" | cut -d ' ' -f4)
	NUMBER_VSZ=$(echo "${process}" | cut -d ' ' -f2)
	NUMBER_RSS=$(echo "${process}" | cut -d ' ' -f3)

	awk '{ print $1, $3/$2 }'
done< <(ps -e -o user,vsz,rss,pid | grep "^${USER_NAME} " | tr -s ' ' | sort -n -r -k2)
