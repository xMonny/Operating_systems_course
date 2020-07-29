#!/bin/bash

printf "%s\n" "$(join -t ',' <(sort province.csv) <(join -t ',' <(sort city_province.csv) <(sort spread.csv) \
		 | awk -F ',' '{ print $2,$4,$5 }' \
		 | sed 's/ /,/g' \
		 | sort) \
		 | awk -F ',' '$1!=p{ print p, f, s, m; p=$1; f=$2; s=0; m=0} {s+=$3; m+=$4} END { print p, f, s, m }' \
		 | awk -F ' ' '{ print $1 echo " ("$2"): " int(($4/$3)*1000) }' \
		 | sort -nr -t ':' -k2 \
		 | head -n 10)"
