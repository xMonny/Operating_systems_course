#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

FILE="${1}"

if [ ! -f "${FILE}" ]; then
	echo "First argument should be a file"
	exit 2
fi

if [ ! -r "${FILE}" ]; then
	echo "Input file cannot be read"
	exit 3
fi

cat "${FILE}" | sed 's/.* г. - //1' | awk '{ print NR".",$0 }' | sort -t ' ' -k2

#cat "${FILE}" | cut -d ' ' -f4- | awk '{ print NR".", $0 }' | sort -t ' ' -k2

#TEMP_FILE=$(mktemp)
#LINE=1

#while read book
#do
#	REMOVED_YEAR=$(echo "${book}" | cut -d ' ' -f4-) 
#	echo "${LINE}. ${REMOVED_YEAR}" >> "${TEMP_FILE}"
#	LINE=$(("${LINE}"+1))
#done< <(cat "${FILE}")

#cat "${TEMP_FILE}" | sort -t ' ' -k2

#rm "${TEMP_FILE}"
