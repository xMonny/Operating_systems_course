#!/bin/bash

if [ "$#" -ne 0 ]; then
	echo "No arguments needed"
	exit 1
fi

cat philip-j-fry.txt | egrep "[02468]+" | grep -v "[a-w]" | wc - l
