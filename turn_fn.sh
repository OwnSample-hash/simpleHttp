#!/bin/sh

for i in $(echo $1 | rev | tr '/' '\n' | rev);do
    echo $i
    exit
done
