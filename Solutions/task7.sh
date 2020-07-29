#!/bin/bash

if [ "$#" -ne 0 ]; then
	echo "No arguments needed"
	exit 1
fi

cat newPasswd | grep "/Inf/" | cut -d ':' -f1,5 | sed 's/,.*$//1' | grep "a$" | cut -d ':' -f1 | sed 's/..//1' | sed 's/..$//1' | sort | uniq -c
