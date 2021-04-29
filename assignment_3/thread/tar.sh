#!/bin/bash

dir="thread"

if [[ ! -f ~/.sid ]]; then
    read -p "Enter your 10-digit student ID: " sid
    echo "$sid" > ~/.sid
fi

sid=`tail -1 ~/.sid`

if [[ `echo "${PWD##*/}"` == $dir ]]; then
    rm -f $sid.tar; make clean; cd ../; tar cvf $sid.tar --exclude='thread/data' $dir; mv $sid.tar $dir/
else
    echo "Error: $0 must run in the $dir/ directory"
fi

