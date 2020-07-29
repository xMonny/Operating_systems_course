#!/bin/bash

read -p "Input full name of a directory: " directory

if [ ! -d "${directory}" ]; then
	echo "Invalid name of a directory"
	exit 1
fi

find_files=$(find "${directory}" -type f 2>/dev/null | wc -l)
find_directories=$(find "${directory}" -type d 2>/dev/null | wc -l)
find_all=$(find "${directory}" 2>/dev/null | wc -l)
find_all_only_in_directory=$(find "${directory}" -mindepth 1 -maxdepth 1 2>/dev/null | wc -l)
echo "Number of all files in ${directory}: ${find_files}"
echo "Number of all directories in ${directory}: ${find_directories}"
echo "Number of all together: ${find_all}"
echo "Number of all files and directories only in ${directory}: ${find_all_only_in_directory}"

