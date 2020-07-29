#!/bin/bash

read -p "Input a name of user: " inputed_name
active_sessions=$(who | cut -d ' ' -f1 | grep "^${inputed_name}$" | wc -l)
echo "Active sessions of user ${inputed_name}: ${active_sessions}"
