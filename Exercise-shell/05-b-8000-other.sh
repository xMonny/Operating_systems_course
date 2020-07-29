#!/bin/bash

#Напишете shell script, който получава като единствен аргумент име на потребител и за всеки негов процес изписва съобщение за съотношението на RSS към VSZ. Съобщенията да са сортирани, като процесите с най-много заета виртуална памет са най-отгоре.

if [ "$#" -ne 1 ]; then
	echo "You only have to type a username as a first argument"
	exit 1
fi

CHECK=$(cat /etc/passwd | grep '^'"${1}"':')

if [ -z "${CHECK}" ]; then
	echo "No found username ${1}"
	exit 1
fi

USERNAME="${1}"

NUMBER=$(ps --no-header -U "${USERNAME}" -u "${USERNAME}" u | wc -l)

if [ "${NUMBER}" -ne 0 ]; then
	echo "User: Command: RSS/VSZ"

	ps --no-header  -U "${USERNAME}" -u "${USERNAME}" u --sort=-vsz | 
	awk '{ if($6!=0) print $1":",$11":", $6/$5; if($6==0) print $1":",$11":", 0 }'

else
	echo "No proccesses"
fi
