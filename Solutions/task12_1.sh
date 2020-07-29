#!/bin/bash

if [ "$#" -ne 0 ]; then
	echo "No arguments needed"
	exit 1
fi

INODE_LAST=$(find /home/velin -type f -printf "%T@ %i\n" 2>/dev/null | sort -n | tail -n 1 | cut -d ' ' -f2)

find /home/velin -inum "${INODE_LAST}" -printf "%p %d\n" 2>/dev/null
