#!/bin/bash

if [ "$#" -ne 0 ]; then
	echo "No arguments needed"
	exit 1
fi

find /home/SI -maxdepth 1 -type d -printf "%C@ %p\n" | awk -F ' ' '{ if ($1 > 1551168000 && $1 < 151176100) print $2 }'
