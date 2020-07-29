#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "You have to put only 1 argument"
	exit 1
fi

INPUT="${1}"

#echo "${INPUT}" | grep -q "[^[:alnum:]]" //valid too
echo "${INPUT}" | egrep -q "^[0-9A-Za-z]*$"

if [ "$?" -ne 0 ]; then
	echo "First argument doesn't contain only letters and digits"
	exit 10
fi

echo "First argument contains only letters and digits"

exit 0 
