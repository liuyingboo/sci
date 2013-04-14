#!/bin/bash
#Clean Generated Autotools Files

FULLPATH=$(cd "$(dirname "$0")";pwd)
echo "full path to currently executed script is : ${FULLPATH}"
BASEDIR=`dirname $FULLPATH`
echo "parent dir is $BASEDIR"
cd $BASEDIR
echo "Files in "${BASEDIR}" should be deleted"

FS="aclocal.m4
	autom4te.cache 
	configure 
	COPYING 
	INSTALL 
	NEWS 
	AUTHORS 
	autoscan.log
    autoscan.scan
	ChangeLog 
	config.h.in 
	depcomp 
	install-sh 
	missing
    config.h
    config.h.in~
	config.status
	config.sub
	config.guess
    config.log
    Makefile
    stamp-h1
	ltmain.sh
    Makefile.in
	README"
REMOVEFILES=($FS)

length=${#REMOVEFILES[@]}
#echo $length
#删除项目录下面的Autotools生成的文件
dellen=0
filenotdel="---------------------------------------------"
for((i=0;i<${#REMOVEFILES[@]};i++));do 
    DF=$BASEDIR/${REMOVEFILES[$i]}
	if [ -e $DF ];	then
		rm -rf $DF
		echo $DF " has been deleted"
		dellen=`expr $dellen + 1`
	else
		filenotdel=$filenotdel"\n"${REMOVEFILES[$i]}
	fi
done
    echo -e $filenotdel 
 	echo $dellen"/"$length "was deleted,"

#	删除子目录中的Makefile.in和Makefile,注意加-print打印出删除文件
echo "----------------------------------------------------------"
echo "Delete [Makefile.in] from Project Base "          P-A
echo "----------------------------------------------------------"
arrayA=($(find $BASEDIR -name 'Makefile.in'))
if [ ${#arrayA[@]} == 0 ] ; then
	echo "None"
else
	find $BASEDIR -name 'Makefile.in' -exec rm -rf {} \; -print
fi
echo "----------------------------------------------------------"

echo "----------------------------------------------------------"
echo "Delete [Makefile] from Project Base "             P-B
echo "----------------------------------------------------------"
arrayA=($(find $BASEDIR -name 'Makefile'))
if [ ${#arrayA[@]} == 0 ] ; then
	echo "None"
else
	find $BASEDIR -name 'Makefile' -exec rm -rf {} \; -print
fi
echo "----------------------------------------------------------"

echo "----------------------------------------------------------"
echo "Delete [*.o] from Project Base"                   P-C
echo "----------------------------------------------------------"
arrayA=($(find $BASEDIR -name '*.o'))
if [ ${#arrayA[@]} == 0 ] ; then
	echo "None"
else
	find $BASEDIR -name '*.o' -exec rm -rf {} \; -print
fi
echo "----------------------------------------------------------"

echo "----------------------------------------------------------"
echo "Delete [.deps] from Project Base"                 P-D
echo "----------------------------------------------------------"
arrayA=($(find $BASEDIR -name '.deps'))
if [ ${#arrayA[@]} == 0 ] ; then
	echo "None"
else
	find $BASEDIR -name '.deps' -exec rm -rf {} \; -print
fi
echo "----------------------------------------------------------"
