#!/bin/bash

printf '%s\n' "$(join -t ',' <(sort city.csv) <(sort spread.csv) \
		 | awk -F ',' '$4>=101 { print $1 echo " ("$2"): " int(($5/$4)*1000) }' \
		 | sort -t ':' -nr -k2 \
		 | head -n 10)"
