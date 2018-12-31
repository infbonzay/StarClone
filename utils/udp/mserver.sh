#!/bin/bash

for ((i=0;i<8;i++)) ; do
    ./sender $i &
done
