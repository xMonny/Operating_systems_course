#!/bin/bash
printf '%s\n' "$(join -t ',' <(sort city_province.csv) <(sort spread.csv) \
		 | sort -t ',' -k2 \
		 | awk -F ',' '$2!=p{ NR>1; print p, s, m; p=$2; s=0; m=0} {s+=$4; m+=$5} END { print p, s, m}' \
		 | awk -F ' ' '{ print $1 echo ": " int(($3/$2)*1000) }' \
		 | sort -nr -t ':' -k2 \
		 | head -n 10)"
