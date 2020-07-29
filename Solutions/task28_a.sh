#!/bin/bash

if [ "$#" -ne 0 ]; then
	echo "Invalid number of arguments"
	exit 1;
fi

MAX=0;

while read NUMBER
do
	if [ "${NUMBER}" = "exit" ]; then
		break
	fi

	if ! [ "${NUMBER}" -eq "${NUMBER}" ] 2>/dev/null; then
		continue
	fi

	if [ "${NUMBER}" -lt 0 ]; then
		ABS_NUMBER=$((0-"${NUMBER}"))
		if [ "${ABS_NUMBER}" -ge "${MAX}" ]; then
			MAX="${ABS_NUMBER}"
			REMEMBER_NEGATIVE="${NUMBER}"
		fi
	else
		if [ "${NUMBER}" -ge "${MAX}" ]; then
			MAX="${NUMBER}"
			REMEMBER_MAX_POSITIVE="${NUMBER}"
		fi
	fi
done

ABS_NEGATIVE_NUMBER=$((0-"${REMEMBER_NEGATIVE}"))

if [ "${REMEMBER_MAX_POSITIVE}" -eq "${MAX}" ]; then
	echo "${REMEMBER_MAX_POSITIVE}"
fi

if [ "${ABS_NEGATIVE_NUMBER}" -eq "${MAX}" ]; then
	echo "${REMEMBER_NEGATIVE}"
fi
