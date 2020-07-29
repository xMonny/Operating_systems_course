#!/bin/bash

if [ "$#" -ne 0 ]; then
	echo "No arguments needed"
	exit 1
fi

USER_NAME=$(whoami)

find / -user $(id -un) 2>/dev/null | wc -l

#CNT=0

#while read line
#do
#	OWNER=$(stat "${line}" --printf "%U" 2>/dev/null)

#	if [ "${OWNER}" = "${USER_NAME}" ]; then
#		CNT=$((CNT+1))
#	fi
#done< <(find ~ 2>/dev/null)

#echo "${CNT}"
