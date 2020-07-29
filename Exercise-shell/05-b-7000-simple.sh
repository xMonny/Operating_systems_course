#!/bin/bash

#Да се напише shell скрипт, който приема произволен брой аргументи - имена на файлове. Скриптът да прочита от стандартния вход символен низ и за всеки от зададените 
#файлове извежда по подходящ начин на стандартния изход броя на редовете, които съдържат низа.

# NB! Низът може да съдържа интервал.

if [ "$#" -eq 0 ]; then
	echo "Please, input an argument"
	exit 1
fi

read -p "Input a string: " STRING

for FILE; do
	if [ ! -f "${FILE}" ] 2>/dev/null; then
        echo "Input invalid fail"
        exit 1
    fi

	fgrep "${STRING}" "${FILE}" | wc -l
done

