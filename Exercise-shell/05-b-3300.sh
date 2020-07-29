#!/bin/bash

read -p "Input name of 3 files: " FILE1 FILE2 FILE3

if [ ! -r "${FILE1}" ]; then
	echo "Invalid name of first file"
	exit 1
fi

if [ ! -r "${FILE2}" ]; then
	echo "Invalid name of second file"
	exit 2
fi

if [ -z "${FILE3}" ]; then
	echo "Empty name of third file"
	exit 3
fi

if [ -f "${FILE3}" ]; then
	echo "Note: Output file already exists. It was overwritten"
fi

paste "${FILE1}" "${FILE2}" | sort > "${FILE3}"
