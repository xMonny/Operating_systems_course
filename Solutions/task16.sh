#!/bin/bash

if [ "$#" -ne 3 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

FILE="${1}"
STRING_ONE="${2}"
STRING_TWO="${3}"

if ! [ -f "${FILE}" ]; then
	echo "Invalid name of file"
	exit 2
fi

if ! [ -r "${FILE}" ]; then
	echo "File doesn't have read permissions"
	exit 3
fi

if [ -z "${STRING_ONE}" ]; then
	echo "Second argument should be non-empty"
	exit 4
fi

if [ -z "${STRING_TWO}" ]; then
	echo "Third argument should be non-empty"
	exit 5
fi

FOUND_KEY_ONE=$(cat "${FILE}" | grep "^${STRING_ONE}=")
FOUND_KEY_TWO=$(cat "${FILE}" | grep "^${STRING_TWO}=")

if [ -z "${FOUND_KEY_ONE}" -o -z "${FOUND_KEY_TWO}" ]; then
	echo "One of the input strings is missing from input file"
	exit 6
fi

SET_ONE=$(echo "${FOUND_KEY_ONE}" | cut -d '=' -f2 | sed 's/ /\n/g')

while read symbol
do
	CHECK=$(echo "${FOUND_KEY_TWO}" | grep "=${symbol} ")
	while [ -n "${CHECK}" ]; do
		FOUND_KEY_TWO=$(echo "${FOUND_KEY_TWO}" | sed "s/${symbol} //1")
		CHECK=$(echo "${FOUND_KEY_TWO}" | grep "=${symbol} ")
	done

	CHECK=$(echo "${FOUND_KEY_TWO}" | grep " ${symbol} ")
	while [ -n "${CHECK}" ]; do
		FOUND_KEY_TWO=$(echo "${FOUND_KEY_TWO}" | sed "s/ ${symbol} / /1")
		CHECK=$(echo "${FOUND_KEY_TWO}" | grep " ${symbol} ")
	done

	CHECK=$(echo "${FOUND_KEY_TWO}" | grep " ${symbol}$")
	while [ -n "${CHECK}" ]; do
		FOUND_KEY_TWO=$(echo "${FOUND_KEY_TWO}" | sed "s/ ${symbol}$//1")
		CHECK=$(echo "${FOUND_KEY_TWO}" | grep " ${symbol}$")
	done
done< <(echo "${SET_ONE}")

FOR_CHANGE=$(cat "${FILE}" | grep "^${STRING_TWO}=")
sed -i "s/${FOR_CHANGE}$/${FOUND_KEY_TWO}/g" "${FILE}"
