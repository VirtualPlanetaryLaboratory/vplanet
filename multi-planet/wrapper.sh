#!/usr/bin/env bash

while IFS= read -r line
do
    [[$line =~ ^[[:blank:]]*# ]] && continue
    python multi_process.py $line
done < multi_list.txt
