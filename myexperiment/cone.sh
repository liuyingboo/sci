#!/bin/bash

if [ ! "`whoami`" = "root" ]; then
	echo "Warning: Only root can run this scripts successfully!"
	exit
fi

mysqlcone=0
fastbitcone=0

RA=35
DEC=45
R=4
PI=3.1415926


##############
##MYSQL TIME
##############
cacheoff="set global query_cache_type=off;"
setprofile="set profiling=1;"
showprofile="show profiles;"
##############
## TOTAL VARs
##############
mysqlTotalAvgTime=0
totalcountsql="select count(*) from fitshead;"
totalcount=`mysql -u root -pgsdjsj fits -e "${totalcountsql}"|tail -1 `
##############
echo "----------------------------"
echo "MYSQL cone search:"
echo "----------------------------"
conesql="select t_dateobs,t_ra,t_dec,t_sr,t_ma from fitshead  where ((acos(sin(t_ra*$PI/180)*sin($RA*$PI/180)+cos(t_ra*$PI/180)*cos($RA*$PI/180)*cos((t_dec-$DEC)*$PI/180)))*180/ $PI )<$R;"
echo $conesql
conesqlcount="select count(*) from fitshead  where ((acos(sin(t_ra*$PI/180)*sin($RA*$PI/180)+cos(t_ra*$PI/180)*cos($RA*$PI/180)*cos((t_dec-$DEC)*$PI/180)))*180/$PI )<$R"
#each for ten times

stoConeFile="mysql.tmp.cone."${totalcount}.txt

mysql -u root -pgsdjsj fits -e "\
${cacheoff}\
${setprofile}\
${conesql}\
${conesql}\
${conesql}\
${conesql}\
${conesql}\
${conesql}\
${conesql}\
${conesql}\
${conesql}\
${conesql}\
show profiles;" | tail -10 | tee $stoConeFile

#################
## RETRIVE HITS
#################
mysqlHits=`mysql -u root -pgsdjsj fits -e "${conesqlcount}"|tail -1 | awk '{print $1}'`

################
## COMPUTER AVG.
################
if [ -f $stoConeFile ]; then
	eval $(awk 'BEGIN{sum=0.0};{sum += $2};END{sum=sum/10;printf("mysqlTotalAvgTime=%s",sum);}' $stoConeFile)
fi
################
## STORE RESULT
################
echo "################" >> $stoConeFile
echo "Total query Time:"$mysqlTotalAvgTime >> $stoConeFile
echo "Total query Hits:"$mysqlHits >> $stoConeFile
echo "################" >> $stoConeFile

echo "-------------------------Done---------------------------"

###################################
##FASTBIT
###################################
echo "------------------"
echo "Fastbit "
echo "------------------"
fastbitexam="/home/lyb/softwares/fastbit-ibis1.3.5/examples"

echo "=========================================="
#####################
##FASTBIT PREPROCESS
##FOR FASTBIT 1.3.5
#####################
if [ -f /var/lib/mysql/fits/conesearch.txt ]; then
	rm -rf /var/lib/mysql/fits/conesearch.txt
fi

#export data to a txt file
mysql -u root -pgsdjsj fits -e "select t_dateobs,t_ra,t_dec,t_sr,t_ma from fitshead into outfile 'conesearch.txt' fields terminated by ',';"

rm -rf ${fastbitexam}/conesearch
rm -rf /home/lyb/conesearch.txt

if [ -f /var/lib/mysql/fits/conesearch.txt ]; then
	cp /var/lib/mysql/fits/conesearch.txt /home/lyb/
	chmod +r /home/lyb/conesearch.txt
	${fastbitexam}/ardea -d ${fastbitexam}/conesearch  -m "dateobs:int,ra:float,dec:float,sr:float,ma:float" -t /home/lyb/conesearch.txt 
fi

echo "=========================================="
echo "== Export Result to Folder: $fastbitexam"
echo "=========================================="

rmqry()
{
	if [ -f fastbit.qry ]; then
		rm -rfv fastbit.qry
	fi
}
outputname="fastbit.tmp.cone"
stoConeFile=${outputname}.${totalcount}.txt

rmtmp()
{
#delete tmp files
	if [ -f $stoConeFile ]; then
		rm -rf $stoConeFile
	fi

}

fstHits=0
cputime=0
elapsedtime=0

rmtmp

for (( i=0;i<11;i++))
do

#delete generated files
	rmqry
	${fastbitexam}/ibis -d ${fastbitexam}/conesearch/ -v -q "SELECT dateobs,ra,dec,sr,ma WHERE ((acos(sin(ra*$PI/180)*sin($RA*$PI/180)+cos(ra*$PI/180)*cos($RA*$PI/180)*cos((dec-$DEC)*$PI/180)))*180/ $PI )<$R" 1 > fastbit.qry 2>&1
	
	if [ -f fastbit.qry ]; then 
		fstHits=`head -8 fastbit.qry | tail -1 | sed 's/\(.*\)table (\(.*\)x\(.*\)/\2/g'| sed 's/[ ]*//g'`
		echo "Total Hits:"$fstHits
		cputime=`tail -1 fastbit.qry | sed 's/\(.*\)total CPU time\(.*\) s, total\(.*\)/\2/g'|sed 's/[ ]*//g'`
		echo "Total CPU Time:"$cputime
		elapsedtime=`tail -1 fastbit.qry | sed 's/\(.*\)total elapsed time\(.*\) s\(.*\)/\2/g'|sed 's/[ ]*//g'`	
		echo "Total elapsed Time:"$elapsedtime
		echo "----"

		echo $i $cputime $elapsedtime >> $stoConeFile

	fi

done

echo "-------" >> $stoConeFile
echo "Total Count:"${totalcount} >> $stoConeFile
echo "Total Hits:"${fstHits} >> $stoConeFile

fstTotalAvgCpuTime=0
#########################
##COMPUTER AVG. COUNT
#########################
if [ -f $stoConeFile ]; then
	echo "----"
	eval $(awk 'BEGIN{sum=0.0};{sum += $2};END{sum = sum/10; printf("fstTotalAvgCpuTime=%s",sum);}' $stoConeFile)
	echo "Avg.CPU Time: "$fstTotalAvgCpuTime
	echo "Avg.CPU Time:"$fstTotalAvgCpuTime >> $stoConeFile
fi



fastQueryRst="fastbit.query.hits.txt"
${fastbitexam}/ibis -d ${fastbitexam}/conesearch/ -q $fastbitsql "SELECT count(*) WHERE ((acos(sin(ra*$PI/180)*sin($RA*$PI/180)+cos(ra*$PI/180)*cos($RA*$PI/180)*cos((dec-$DEC)*$PI/180)))*180/ $PI )<4" 1 > $fastQueryRst 2>&1

# if [ -f fastbit.qry ]; then 
# 	fstHits=`head -8 fastbit.qry | tail -1 | sed 's/\(.*\)table (\(.*\)x\(.*\)/\2/g'| sed 's/[ ]*//g'`
# 	echo "Total Hits:"$fstHits
# fi

# if [ ${#fstHits} -gt 15 ]; then
# 	fstHits=`head -8 fastbit.qry | tail -1 | sed 's/\(.*\)the first (\(.*\)rows\(.*\)/\2/g'| sed 's/[ ]*//g'`
# else
# 	fstHits=`tail -2  $fastQueryRst | head -1`
# fi

# if [ -f $fastQueryRst ]; then
# 	if [ ${#fstHits} -gt 15 ]; then
# 		fstHits=`head -8 fastbit.qry | tail -1 | sed 's/\(.*\)the first (\(.*\)rows\(.*\)/\2/g'| sed 's/[ ]*//g'`
# 	else
# 		fstHits=`tail -2  $fastQueryRst | head -1`
# 	fi
# else
# 	fstHits=0
# fi

if [ -f $fastQueryRst ]; then
	fstHits=`head -8 fastbit.qry | tail -1 | sed 's/\(.*\)the first (\(.*\)rows\(.*\)/\2/g'| sed 's/[ ]*//g'`
	if [ ${#fstHits} -gt 15 ]; then
		fstHits=`head -8 fastbit.qry | tail -1 | sed 's/\(.*\)table (\(.*\)x\(.*\)/\2/g'| sed 's/[ ]*//g'`
	else
		fstHits=0		
	fi
else
	fstHits=0
fi


echo -e $totalcount "\t\t\t" $mysqlTotalAvgTime  "\t\t\t" $fstTotalAvgCpuTime "\t\t\t" $mysqlHits "\t\t\t" $fstHits >> MyExperiment.Stat.Cone.txt


