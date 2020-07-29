#!/bin/bash

if [ "$#" -lt 1 -o "$#" -gt 2 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

FIRST="${1}"

if [ -z "${FIRST}" ]; then
	echo "First argument should be non-empty"
	exit 2
fi

RESULT=$(echo "${FIRST}" | grep "^-[rgb]$")

if [ "$#" -eq 1 -a -n "${RESULT}" ]; then
	echo "Input second argument"
elif [ "$#" -eq 1 ]; then
	echo "${FIRST}"
elif [ "$#" -eq 2 -a -z "${RESULT}" ]; then
	echo "Unknown color"
elif [ -n "${RESULT}" ]; then
	SECOND="${2}"
	if [ -z "${SECOND}" ]; then
		echo "Second argument should be non-empty"
		exit 3
	fi

	if [ "${FIRST}" = "-r" ]; then
		echo -e "\033[0;31m ${SECOND}"
	elif [ "${FIRST}" = "-g" ]; then
		echo -e "\033[0;32m ${SECOND}"
	elif [ "${FIRST}" = "-b" ]; then
		echo -e "\033[0;34m ${SECOND}"
	fi
fi

#echo '\033[0m'
