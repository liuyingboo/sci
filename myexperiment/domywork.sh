#!/bin/bash

echo `pwd`

if [ ! "`whoami`" = "root" ]; then
	echo "Warning: Only root can run this scripts successfully!"
	exit
fi


isProc=0
NumberToProc=0
TotalRow=0
CurProc=0
#awk '{printf $0}' ./switch.data
eval $(awk '{printf("TotalRow=%s\n",NR);}' ./switch.data)
echo Total Rows: $TotalRow
for(( i=1;i<=$TotalRow;i++));
do
	case "$i" in
		"1" )
			eval $(awk 'NR == 1  { printf("isProc=%s\nNumberToProc=%s",$1,$2); }' ./switch.data)
			;;
		"2" )
			eval $(awk 'NR == 2 { printf("isProc=%s\nNumberToProc=%s",$1,$2); }' ./switch.data)
			;;
		"3" )
			eval $(awk 'NR == 3 { printf("isProc=%s\nNumberToProc=%s",$1,$2); }' ./switch.data)
			;;
		"4" )
			eval $(awk 'NR == 4 { printf("isProc=%s\nNumberToProc=%s",$1,$2); }' ./switch.data)
			;;
		"5" )
			eval $(awk 'NR == 5 { printf("isProc=%s\nNumberToProc=%s",$1,$2); }' ./switch.data)
			;;
		"6" )
			eval $(awk 'NR == 6 { printf("isProc=%s\nNumberToProc=%s",$1,$2); }' ./switch.data)
			;;
		"7" )
			eval $(awk 'NR == 7 { printf("isProc=%s\nNumberToProc=%s",$1,$2); }' ./switch.data)
			;;
		"*" )
			isProc="0END"
			NumberToProc="0END"
	esac   
	echo $isProc $NumberToProc
	
	if [ $isProc"x" == "1x" ]; then
		CurProc=$i
		break
 	fi
done

if [ $isProc"x" == "0x" ]; then
	echo "======================="	
	echo "No More Task to be run!"
	echo "======================="
	echo ""
	exit
fi

NR=$NumberToProc

echo "==================="
echo "Processing Rows:" $NR
echo "==================="
mysql -u root -pgsdjsj fits -e "delete from fitshead"
/home/lyb/myexperiment/massdata -n ${NR}
bash /home/lyb/myexperiment/runmyexperi.sh

echo "Write Back to File: [$NR]"
# echo ${CurProc}
# echo "----------"
#sed -n "${CurProc}p" ./switch.data

if [ $CurProc == 0 ];then
	echo "All Data Has Been Processed! No More Data! Dont Do Again!"
else
	sed -e "${CurProc}s/^1/0/" ./switch.data > ./switch.data.tmp
	rm -rf ./switch.data
	mv ./switch.data.tmp ./switch.data
fi



echo "====ALL TEST WORKS HAVE BEEN DONE===="
echo "====ALL Works Have been Done!=====" >> result.txt 
