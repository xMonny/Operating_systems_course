#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Expect only a name of a directory"
	exit 1
fi

dir="${1}"
if [ ! -d "${dir}" ]; then
	echo "Expect a directoy"
	exit 2
fi

if [ ! -r "${dir}" ]; then
	echo "Input directory cannot be read"
	exit 3
fi

find "${dir}" -xtype l 2>/dev/null
