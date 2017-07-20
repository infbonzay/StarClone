#!/bin/bash -x
PREVDIR=`pwd`
MYPATH=`dirname $0`
cd "$MYPATH"
MYPATH=`pwd`

env "LD_LIBRARY_PATH=$MYPATH/libs:$LD_LIBRARY_PATH" "$MYPATH/StarClone" "$@"
#env "LD_LIBRARY_PATH=$MYPATH/libs:$LD_LIBRARY_PATH" ldd "$MYPATH/StarClone"
cd "$PREVDIR"