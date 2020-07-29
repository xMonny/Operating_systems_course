#!/bin/bash

if [ "$#" -ne 0 ]; then
	echo "No arguments needed"
	exit 1
fi

cat task10_file | cut -d ':' -f1,5,6 | fgrep "/SI/" | cut -c 2- | sort -n | cut -d ':' -f2,3 | sed -E "s/,.+:/:/1"
