#!/bin/bash
printf '%s\n' "$(cat spread.csv \
		 | awk -F ',' '$3>=101 { print $1 echo ": " int(($4/$3)*1000) }'\
		 | sort -nr -t ':' -k2 | head -n 10)" 
