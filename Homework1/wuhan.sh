#!/bin/bash
echo $(grep $(grep 'Wuhan$' city.csv | cut -d ',' -f1) spread.csv | cut -d ',' -f4)
