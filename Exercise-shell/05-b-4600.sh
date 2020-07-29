#!/bin/bash

if [ "$#" -ne 3 ]; then
	echo "Expect 3 arguments"
	exit 10
fi

if ! [ "${1}" -eq "${1}" -o "${2}" -eq "${2}" -o "${3}" -eq "${3}" ] 2>/dev/null; then
	exit 3
fi

NUMBER="${1}"
LEFT="${2}"
RIGHT="${3}"

if [ "${LEFT}" -ge "${RIGHT}" ]; then
	exit 2
fi

if [ "${LEFT}" -gt "${NUMBER}" -o "${NUMBER}" -gt "${RIGHT}" ]; then
	exit 1
fi
