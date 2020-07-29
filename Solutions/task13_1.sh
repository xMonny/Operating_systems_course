#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Expect only a name of a directory"
	exit 1
fi

DIRECTORY="${1}"

if ! [ -d "${DIRECTORY}" ]; then
	echo "Invalid name of directory"
	exit 2
fi

if [ ! -r "${DIRECTORY}" -o ! -x "${DIRECTORY}" ]; then
	echo "Directory doesn't have permissions"
	exit 3
fi

find "${DIRECTORY}" -xtype l 2>/dev/null
