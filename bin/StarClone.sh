#!/bin/bash

PREVDIR=`pwd`
MYPATH=`dirname $0`

cd "$MYPATH"
MYPATH=`pwd`
env "LD_LIBRARY_PATH=$MYPATH/libs:$LD_LIBRARY_PATH" "$MYPATH/StarClone" "$@"
cd "$PREVDIR"