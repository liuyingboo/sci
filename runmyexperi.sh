#!/bin/bash

if [ ! "`whoami`" = "root" ]; then
	echo "Warning: Only root can run this scripts successfully!"
	exit
fi
cacheoff="set global query_cache_type=off;"
setprofile="set profiling=1;"
showprofile="show profiles;"
dateobsrange="select min(t_dateobs),max(t_dateobs) from fitshead;"
totalcountsql="select count(*) from fitshead;"
lower=`mysql -u root -pgsdjsj fits -e "${dateobsrange}"|tail -1 | awk '{print $1}'`
upper=`mysql -u root -pgsdjsj fits -e "${dateobsrange}"|tail -1 | awk '{print $2}'`

##############
totalcount=`mysql -u root -pgsdjsj fits -e "${totalcountsql}"|tail -1 `
echo "Total Records:"$totalcount

##############
echo "min(t_dateobs):"$lower
echo "max(t_dateobs):"$upper
onelevel=`echo "$lower + ($upper-$lower) * 10/100" | bc`
twolevel=`echo "$lower + ($upper-$lower) * 90/100" | bc`
threelevel=`echo "$lower + ($upper-$lower) * 40/100" | bc`
fourlevel=`echo "$lower + ($upper-$lower) * 60/100" | bc`
echo "[1/10 of t_dateobs:]"$onelevel
echo "[9/10 of t_dateobs:]"$twolevel

##############
echo "----------------------------"
echo "ONE SIDE QUERY:"
echo "----------------------------"
onesidesql="select * from fitshead where t_dateobs < $onelevel ;"
onesidesqlcount="select count(*) from fitshead where t_dateobs < $onelevel ;"
#each for ten times
sqlname="mysql.tmp"
mysql -u root -pgsdjsj fits -e "\
${cacheoff}\
${setprofile}\
${onesidesql}\
${onesidesql}\
${onesidesql}\
${onesidesql}\
${onesidesql}\
${onesidesql}\
${onesidesql}\
${onesidesql}\
${onesidesql}\
${onesidesql}\
show profiles;" | tail -10 | tee ${sqlname}.${totalcount}.1side.txt

if [ -f ${sqlname}.${totalcount}.1side.txt ]; then
	awk 'BEGIN{sum=0.0};{sum += $2};END{print "Avg.Query(10-Times):" sum / 10 "(s)"}' ${sqlname}.${totalcount}.1side.txt 
fi

totalCount=`mysql -u root -pgsdjsj fits -e "${onesidesqlcount}"|tail -1 | awk '{print $1}'`
echo "Total query Hits:"$totalCount
echo "----" >> ${sqlname}.${totalcount}.1side.txt
echo "Total query Hits:"$totalCount >> ${sqlname}.${totalcount}.1side.txt
##############
echo "----------------------------"
echo "TWO SIDE QUERY:"
echo "----------------------------"
twosidesql="select * from fitshead where t_dateobs < $onelevel or t_dateobs > $twolevel ;"
twosidesqlcount="select count(*) from fitshead where t_dateobs < $onelevel or t_dateobs > $twolevel ;"

mysql -u root -pgsdjsj fits -e "\
${cacheoff}\
${setprofile}\
${twosidesql}\
${twosidesql}\
${twosidesql}\
${twosidesql}\
${twosidesql}\
${twosidesql}\
${twosidesql}\
${twosidesql}\
${twosidesql}\
${twosidesql}\
show profiles;" | tail -10 | tee ${sqlname}.${totalcount}.or.txt

if [ -f ${sqlname}.${totalcount}.or.txt ]; then
	awk 'BEGIN{sum=0.0};{sum += $2};END{print "Avg.Query(10-Times):" sum / 10 "(s)"}' ${sqlname}.${totalcount}.or.txt 
fi
totalCount=`mysql -u root -pgsdjsj fits -e "${twosidesqlcount}"|tail -1 | awk '{print $1}'`
echo "Total query Hits:"$totalCount
echo "----" >> ${sqlname}.${totalcount}.or.txt
echo "Total query Hits:"$totalCount >> ${sqlname}.${totalcount}.or.txt
##############
echo "----------------------------"
echo "TWO SIDE QUERY(BETWEEN):"
echo "----------------------------"
twoandsidesql="select * from fitshead where t_dateobs > $threelevel and t_dateobs < $fourlevel ;"
twoandsidesqlcount="select count(*) from fitshead where t_dateobs > $threelevel and t_dateobs < $fourlevel ;"

mysql -u root -pgsdjsj fits -e "\
${cacheoff}\
${setprofile}\
${twoandsidesql}\
${twoandsidesql}\
${twoandsidesql}\
${twoandsidesql}\
${twoandsidesql}\
${twoandsidesql}\
${twoandsidesql}\
${twoandsidesql}\
${twoandsidesql}\
${twoandsidesql}\
show profiles;" | tail -10 | tee ${sqlname}.${totalcount}.and.txt

if [ -f ${sqlname}.${totalcount}.and.txt ]; then
	awk 'BEGIN{sum=0.0};{sum += $2};END{print "Avg.Query(10-Times):" sum / 10 "(s)"}' ${sqlname}.${totalcount}.and.txt 
fi
totalCount=`mysql -u root -pgsdjsj fits -e "${twoandsidesqlcount}"|tail -1 | awk '{print $1}'`
echo "Total query Hits:"$totalCount
echo "----" >> ${sqlname}.${totalcount}.and.txt
echo "Total query Hits:"$totalCount >> ${sqlname}.${totalcount}.and.txt


###################################
##FASTBIT
###################################
echo "------------------"
echo "Fastbit "
echo "------------------"
fastbitexam="/home/lyb/softwares/fastbit-ibis1.3.5/examples"

echo "=========================================="

if [ -f /var/lib/mysql/fits/conesearch.txt ]; then
	rm -rfv /var/lib/mysql/fits/conesearch.txt
fi

#export data to a txt file
mysql -u root -pgsdjsj fits -e "select t_dateobs,t_ra,t_dec,t_sr,t_ma from fitshead into outfile 'conesearch.txt' fields terminated by ',';"

rm -rfv ${fastbitexam}/conesearch
rm -rfv /home/lyb/conesearch.txt

if [ -f /var/lib/mysql/fits/conesearch.txt ]; then
	cp /var/lib/mysql/fits/conesearch.txt /home/lyb/
	chmod +r /home/lyb/conesearch.txt
	${fastbitexam}/ardea -d ${fastbitexam}/conesearch  -m "dateobs:int,ra:float,dec:float,sr:float,ma:float" -t /home/lyb/conesearch.txt
fi
echo "=========================================="

###############################################
###Fastbit One Side
###############################################
echo "================One Side==================="
rmqry()
{
	if [ -f fastbit.qry ]; then
		rm -rfv fastbit.qry
	fi
}
outputname="fastbit.tmp.1side"
rmtmp()
{
	#delete tmp files
	if [ -f ${outputname}.${totalcount}.txt ]; then
		rm -rf ${outputname}.${totalcount}.txt
	fi

}

fasthits=0
cputime=0
elapsedtime=0
	
rmtmp
for (( i=0;i<10;i++))
do

	#delete generated files
	rmqry
	${fastbitexam}/ibis -d ${fastbitexam}/conesearch/ -v -q "select dateobs,ra,dec,sr,ma  where dateobs < $onelevel" 1 > fastbit.qry 2>&1

	if [ -f fastbit.qry ]; then
		fasthits=`tail -2 fastbit.qry | head -1 | sed 's/\(.*\)produced\(.*\)hits\(.*\)/\2/g'|sed 's/[ ]*//g'`
		echo "Total Hits:"$fasthits
		cputime=`tail -1 fastbit.qry | sed 's/\(.*\)total CPU time\(.*\) s, total\(.*\)/\2/g'|sed 's/[ ]*//g'`
		echo "Total CPU Time:"$cputime
		elapsedtime=`tail -1 fastbit.qry | sed 's/\(.*\)total elapsed time\(.*\) s\(.*\)/\2/g'|sed 's/[ ]*//g'`	
		echo "Total elapsed Time:"$elapsedtime
		echo "----"
		#save to a file
		echo $i $cputime $elapsedtime >> ${outputname}.${totalcount}.txt

	fi

done

echo "-------" >> ${outputname}.${totalcount}.txt
echo "Total Count:"${totalcount} >> ${outputname}.${totalcount}.txt
echo "Total Hits:"${fasthits} >> ${outputname}.${totalcount}.txt
echo ${fastbitexam}/ibis -d ${fastbitexam}/conesearch/ -v -q "select dateobs,ra,dec,sr,ma  where dateobs < $onelevel" >> ${outputname}.${totalcount}.txt.txt
fastavgcputime=0

if [ -f ${outputname}.${totalcount}.txt ]; then
	echo "----"
	eval $(awk 'BEGIN{sum=0.0};{sum += $2};END{sum = sum/10; printf("fastavgcputime=%s",sum);}' ${outputname}.${totalcount}.txt)
	echo "Avg.CPU Time: "$fastavgcputime
	echo "Avg.CPU Time:"$fastavgcputime >> ${outputname}.${totalcount}.txt
fi


###############################################
###Fastbit or Side
###############################################
echo "================OR==================="
rmqry()
{
	if [ -f fastbit.qry ]; then
		rm -rfv fastbit.qry
	fi
}
outputname="fastbit.tmp.or"
rmtmp()
{
	#delete tmp files
	if [ -f ${outputname}.${totalcount}.txt ]; then
		rm -rf ${outputname}.${totalcount}.txt
	fi

}

fasthits=0
cputime=0
elapsedtime=0
	
rmtmp
for (( i=0;i<10;i++))
do

	#delete generated files
	rmqry
	${fastbitexam}/ibis -d ${fastbitexam}/conesearch/ -v -q "select dateobs,ra,dec,sr,ma  where dateobs < $onelevel or dateobs > $twolevel " 1 > fastbit.qry 2>&1

	if [ -f fastbit.qry ]; then
		fasthits=`tail -2 fastbit.qry | head -1 | sed 's/\(.*\)produced\(.*\)hits\(.*\)/\2/g'|sed 's/[ ]*//g'`
		echo "Total Hits:"$fasthits
		cputime=`tail -1 fastbit.qry | sed 's/\(.*\)total CPU time\(.*\) s, total\(.*\)/\2/g'|sed 's/[ ]*//g'`
		echo "Total CPU Time:"$cputime
		elapsedtime=`tail -1 fastbit.qry | sed 's/\(.*\)total elapsed time\(.*\) s\(.*\)/\2/g'|sed 's/[ ]*//g'`	
		echo "Total elapsed Time:"$elapsedtime
		echo "----"
		#save to a file
		echo $i $cputime $elapsedtime >> ${outputname}.${totalcount}.txt

	fi

done

echo "-------" >> ${outputname}.${totalcount}.txt
echo "Total Count:"${totalcount} >> ${outputname}.${totalcount}.txt
echo "Total Hits:"${fasthits} >> ${outputname}.${totalcount}.txt
echo ${fastbitexam}/ibis -d ${fastbitexam}/conesearch/ -v -q "select dateobs,ra,dec,sr,ma  where dateobs < $onelevel or dateobs > $twolevel" >> ${outputname}.${totalcount}.txt
fastavgcputime=0

if [ -f ${outputname}.${totalcount}.txt ]; then
	echo "----"
	eval $(awk 'BEGIN{sum=0.0};{sum += $2};END{sum = sum/10; printf("fastavgcputime=%s",sum);}' ${outputname}.${totalcount}.txt)
	echo "Avg.CPU Time: "$fastavgcputime
	echo "Avg.CPU Time:"$fastavgcputime >> ${outputname}.${totalcount}.txt
fi


###############################################
###Fastbit AND Side
###############################################
echo "================AND==================="
rmqry()
{
	if [ -f fastbit.qry ]; then
		rm -rfv fastbit.qry
	fi
}
outputname="fastbit.tmp.and"
rmtmp()
{
	#delete tmp files
	if [ -f ${outputname}.${totalcount}.txt ]; then
		rm -rf ${outputname}.${totalcount}.txt
	fi

}

fasthits=0
cputime=0
elapsedtime=0
	
rmtmp
for (( i=0;i<10;i++))
do

	#delete generated files
	rmqry
	${fastbitexam}/ibis -d ${fastbitexam}/conesearch/ -v -q "select dateobs,ra,dec,sr,ma  where dateobs > $threelevel and dateobs < $fourlevel " 1 > fastbit.qry 2>&1

	if [ -f fastbit.qry ]; then
		fasthits=`tail -2 fastbit.qry | head -1 | sed 's/\(.*\)produced\(.*\)hits\(.*\)/\2/g'|sed 's/[ ]*//g'`
		echo "Total Hits:"$fasthits
		cputime=`tail -1 fastbit.qry | sed 's/\(.*\)total CPU time\(.*\) s, total\(.*\)/\2/g'|sed 's/[ ]*//g'`
		echo "Total CPU Time:"$cputime
		elapsedtime=`tail -1 fastbit.qry | sed 's/\(.*\)total elapsed time\(.*\) s\(.*\)/\2/g'|sed 's/[ ]*//g'`	
		echo "Total elapsed Time:"$elapsedtime
		echo "----"
		#save to a file
		echo $i $cputime $elapsedtime >> ${outputname}.${totalcount}.txt

	fi

done

echo "-------" >> ${outputname}.${totalcount}.txt
echo "Total Count:"${totalcount} >> ${outputname}.${totalcount}.txt
echo "Total Hits:"${fasthits} >> ${outputname}.${totalcount}.txt
echo ${fastbitexam}/ibis -d ${fastbitexam}/conesearch/ -v -q "select dateobs,ra,dec,sr,ma  where dateobs > $threelevel and dateobs < $fourlevel " >> ${outputname}.${totalcount}.txt
fastavgcputime=0

if [ -f ${outputname}.${totalcount}.txt ]; then
	echo "----"
	eval $(awk 'BEGIN{sum=0.0};{sum += $2};END{sum = sum/10; printf("fastavgcputime=%s",sum);}' ${outputname}.${totalcount}.txt)
	echo "Avg.CPU Time: "$fastavgcputime
	echo "Avg.CPU Time:"$fastavgcputime >> ${outputname}.${totalcount}.txt
fi
