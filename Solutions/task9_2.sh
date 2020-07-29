#!/bin/bash

if [ $# -ne 0 ]; then
	echo "err"
	exit 3
fi

a=$(cat /etc/passwd | egrep "^pesho:" | cut -d ':' -f6) 
find "${a}" -type f -printf "%T@ %p %i\n" 2>/dev/null | sort -nr | head -n 1 | cut -d ' ' -f3  


