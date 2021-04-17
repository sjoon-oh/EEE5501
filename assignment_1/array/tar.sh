#!/bin/bash

dir="array"

if [[ ! -f ~/.sid ]]; then
    read -p "Enter your 10-digit student ID: " sid
    echo "$sid" > ~/.sid
fi

sid=`tail -1 ~/.sid`

if [[ `basename $PWD` == $dir ]]; then
    make clean; cd ../; tar cvf $sid.tar $dir; mv $sid.tar $dir/
else
    echo "Error: $0 must run in the $dir/ directory"
fi

