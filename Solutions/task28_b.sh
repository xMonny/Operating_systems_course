#!/bin/bash

if [ "$#" -ne 0 ]; then
	echo "No arguments needed"
	exit 1
fi

SUM=0
MAX_SUM=0
CNT=0

while read NUMBER
do
	if [ -z "${NUMBER}" ]; then
		break
	fi

	if ! [ "${NUMBER}" -eq "${NUMBER}" ] 2>/dev/null; then
		continue
	fi

	REMEMBER_NUMBER="${NUMBER}"
	
	while [ "${REMEMBER_NUMBER}" -ne 0 ]
	do
		DIGIT=$(("${REMEMBER_NUMBER}"%10))
		SUM=$((SUM+"${DIGIT}"))
		REMEMBER_NUMBER=$(("${REMEMBER_NUMBER}"/10))		
	done

	if [ "${CNT}" -eq 0 ]; then
		MAX_SUM="${SUM}"
		MINIMAL="${NUMBER}"
		CNT=$((CNT+1))
		continue
	fi

	if [ "${SUM}" -ge "${MAX_SUM}" -a "${NUMBER}" -lt "${MINIMAL}" ]; then
		MAX_SUM="${SUM}"
		MINIMAL="${NUMBER}"
	fi
done

echo "${MINIMAL}"
