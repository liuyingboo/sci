#!/bin/bash

FULLPATH=$(cd "$(dirname "$0")";pwd)
echo "full path to currently executed script is : ${FULLPATH}"
BASEDIR=`dirname $FULLPATH`
echo "parent dir is $BASEDIR"

cd $BASEDIR
ls -lsh

echo "!!!Current Working Directory: ${BASEPATH}"

#libtoolize --force
aclocal
autoheader
autoconf
touch AUTHORS NEWS README ChangeLog
automake --add-missing --copy --gnu
#######################################
##In order to debug macros
#######################################
./configure CFLAGS="-gdwarf-2 -g3 -O0" 

