#!/bin/bash
dirname=$1 ;
num_of_files=$2 ;
num_of_dirs=$3 ;
levels=$4 ;
echo $dirname $num_of_files $num_of_dirs $levels
# Print message if dir exists , else create it
if [[ -d $dirname ]]; then
    echo "Directory Exists"
else
    mkdir $dirname
fi
if [[ $num_of_files -lt 0 ]]; then
    echo "Incorrect num of files $num_of_files "
fi
if [[ $num_of_dirs -lt 0 ]]; then
    echo "Incorrect num of files $num_of_dirs "
fi
if [[ $levels -lt 0 ]]; then
    echo "Incorrect num of files $levels "
fi
# Commands for random generation found at
# https://gist.github.com/earthgecko/3089509
# First we pick a random number from 1-8 fro the size  of the title
CURPATH="$dirname"
PATHS=( $dirname )
lvl=$((0))
for (( i = 0; i < $num_of_dirs; i++ )); do
    size=`cat /dev/urandom | tr -dc '1-8' | fold -w 256 | head -n 1 | head --bytes 1`
    # then  we pick a random alphanumeric title
    name=`cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $size | head -n 1`
    mkdir "$CURPATH/$name"
    PATHS+=("$CURPATH/$name")
    lvl=$(($lvl + 1))
    if [[ "$lvl" -eq "$levels" ]]; then
        lvl=$((0))
        CURPATH="$dirname"
    else
        CURPATH="$CURPATH/$name"
    fi
done
j=0
for (( i = 0; i < num_of_files; i++ )); do
    size_title=`cat /dev/urandom | tr -dc '1-8' | fold -w 256 | head -n 1 | head --bytes 1`
    # then  we pick a random alphanumeric title
    name=`cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $size_title | head -n 1`
    size_file=$(($RANDOM % 127))
    size_file=$(($size_file * 1024))
    file=`cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $size_file | head -n 1`
    #touch "${PATHS[$j]}/$name"
    echo $file > "${PATHS[$j]}/$name"
    j=$(($j + 1))
    if [[ "$j" -eq "$(($num_of_dirs + 1))" ]]; then
        j=$((0))
    fi
done
