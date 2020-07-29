#!/bin/bash

if [ "$#" -lt 1 -o "$#" -gt 2 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

NUMBER="${1}"

if ! [ "${NUMBER}" -eq "${NUMBER}" ] 2>/dev/null; then
	echo "First argument should be a number"
	exit 2
fi

if [ "$#" -eq 2 ]; then
	DELIMITER="${2}"
else
	DELIMITER=" ";
fi

echo "${DELIMITER}" | grep -q "^.$"

if [ "$?" -ne 0 ]; then
	echo "Second argument should be a character"
	exit 3
fi

FINAL_NUMBER=$(echo "${NUMBER}" \
	       | rev \
	       | grep -o . \
	       | xargs -n3 \
	       | sed 's/ //g' \
	       | tr '\n' "${DELIMITER}" \
	       | sed "s/${DELIMITER}$//g" \
	       | rev \
	       | sed 's/$/\n/g')

REST=0
if [ "${NUMBER}" -lt 0 ]; then
	ROWS=$(echo "${NUMBER}" | grep -o . | wc -l)
	REST=$((ROWS%3))
fi

if [ "${REST}" -eq 1 ]; then #include the symbol '-'
	FINAL_NUMBER=$(echo "${FINAL_NUMBER}" | sed "s/^../-/1")
fi

if [ "${DELIMITER}" = "^" ]; then
	FINAL_NUMBER=$(echo "${FINAL_NUMBER}" | sed "s/\^//1")
fi

echo "${FINAL_NUMBER}"

