#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Expect only 1 argument"
	exit 1
fi

COLOR="${1}"

if [ -z "${COLOR}" ]; then
	echo "Argument should be non-empty"
	exit 2
fi

echo "${COLOR}" | grep -q "^-[rgbx]$"

if [ "$?" -ne 0 ]; then
	echo "Argument should be '-r', '-g', -b' or '-x'"
	exit 3
fi

while read -p "Input a string: " STRING
do
	if [ "${STRING}" = "exit" ]; then
		echo "[exited]"
		exit 0
	fi

	if [ "${COLOR}" = "-x" ]; then
		echo "${STRING}"
	elif [ "${COLOR}" = "-r" ]; then
		echo -e "\033[0;31m ${STRING}"
		COLOR="-g"
	elif [ "${COLOR}" = "-g" ]; then
		echo -e "\033[0;32m ${STRING}"
		COLOR="-b"
	elif [ "${COLOR}" = "-b" ]; then
		echo -e "\033[0;34m ${STRING}"
		COLOR="-r"
	fi

	echo -ne "\033[0m"
done
