#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "error args"
	exit 2
fi

CNT=1

TEMP_FILE=$(mktemp)

while read line
do
	echo "${CNT}. ${line}" >> "${TEMP_FILE}"
	CNT=$((CNT+1))
done< <(cat "${1}" | cut -d ' ' -f4-)

cat "${TEMP_FILE}" | sort -t ' ' -k2

rm "${TEMP_FILE}"	
