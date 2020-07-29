#!/bin/bash

if [ "$#" -ne 3 ]; then
	echo "Expected 3 arguments"
	exit 1
fi

FILE="${1}"

if [ ! -r "${FILE}" ]; then
	echo "Cannot read file ${FILE}"
	exit 2
fi

REAL_NAME="${2}"
NICKNAME="${3}"

if [ -z "${REAL_NAME}" ]; then
	echo "Empty name is input"
	exit 3
fi

if [ -z "${NICKNAME}" ]; then
	echo "Empty nickname is input"
	exit 4
fi

FOUND_USERS=$(cat passwd.txt | grep "${REAL_NAME}[,|:]" | wc -l)

if [ "${FOUND_USERS}" -eq 0 ]; then
	echo "No found users with name ${REAL_NAME}"
	exit 0
elif [ "${FOUND_USERS}" -eq 1 ]; then
	USER_NAME=$(cat passwd.txt | grep "${REAL_NAME}[,|:]" | cut -d ':' -f1)
	echo "${NICKNAME} ${USER_NAME}" >> "${FILE}"
	exit 0
fi

ROW=1
TEMP_FILE=$(mktemp)
while read LINE
do
	echo "${ROW}. ${LINE}"
	echo "${ROW}. ${LINE}" >> "${TEMP_FILE}"
	ROW=$((ROW+1))
done< <(cat passwd.txt | grep "${REAL_NAME}[,|:]")

read -p "Choose a number: " NUMBER

if [ "${NUMBER}" -eq 0 ]; then
	exit 0
fi

CHOSEN_NUMBER=$(cat "${TEMP_FILE}" | grep "^${NUMBER}. ")

if [ -z "${CHOSEN_NUMBER}" ]; then
	echo "Invalid number is chosen"
	exit 6
else
	USER_NAME=$(echo "${CHOSEN_NUMBER}" | cut -d ' ' -f2 | cut -d ':' -f1)
	echo "${NICKNAME} ${USER_NAME}" >> "${FILE}"
fi

rm "${TEMP_FILE}"
