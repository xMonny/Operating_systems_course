#!/bin/bash

if [ $(whoami) == monny ]; then
	TEXT="echo \"Hello, Simona :)\""
else
	TEXT="echo \"Welcome to acer-nitro5 machine\""
fi

echo "${TEXT}" > ~/.bash_profile
