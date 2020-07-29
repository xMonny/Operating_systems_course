#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

LEFT="${1}"
RIGHT="${2}"

if ! [ "${LEFT}" -eq "${LEFT}" ] 2>/dev/null; then
	echo "First argument should be a left bound of interval"
	exit 2
fi

if ! [ "${RIGHT}" -eq "${RIGHT}" ] 2>/dev/null; then
	echo "Second argument should be a right bound of interval"
	exit 3
fi

if [ "${LEFT}" -ge "${RIGHT}" ]; then
	echo "Incorrect interval"
	exit 4
fi

RANDOM_NUMBER=$(( RANDOM % ("${RIGHT}"-"${LEFT}") + "${LEFT}" ))

while read -p "Try to guess a number: " input
do
	if ! [ "${input}" -eq "${input}" ] 2>/dev/null; then
		echo "Invalid number"
		continue
	fi

	if [ "${input}" -lt "${LEFT}" -o "${input}" -gt "${RIGHT}" ]; then
		echo "Out of interval"
		continue
	fi

	if [ "${input}" -lt "${RANDOM_NUMBER}" ]; then
		echo "High"
	fi

	if [ "${input}" -gt "${RANDOM_NUMBER}" ]; then
		echo "Low"
	fi

	if [ "${input}" -eq "${RANDOM_NUMBER}" ]; then
		echo "Congratulations! You guessed the number!"
		exit 0
	fi
done
