#!/bin/bash

if [ "$#" -ne 0 ]; then
	echo "err"
	exit 6
fi

a=$(cat /etc/passwd | egrep "^velin:" | cut -d ':' -f6)

b=$(find "${a}" -type f -printf "%T@ %i\n" 2>/dev/null | sort -nr | head -n 1 |cut -d ' ' -f2)
find "${a}" -type f -inum "${b}" -printf "%d\n" 2>/dev/null | sort -n | head -n 1


  
