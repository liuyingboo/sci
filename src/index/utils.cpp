#include "utils.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>


void replaceStr(std::string & str, const std::string ori,const std::string ne)
{
    for (size_t pos = 0; (pos = str.find(ori)) != std::string::npos;pos += strlen(ne.c_str()))
    {
        str.replace(pos, strlen(ori.c_str()), ne);
    }

}
int getSystemTime() 
{ 
    time_t timer; 
    struct tm* t_tm; 
    time(&timer); 
    t_tm = localtime(&timer); 

    return 0; 
} 

void getLocalTime(char *str) {
    time_t sec = time(0); // current time in seconds

    struct tm stm;
    if (localtime_r(&sec, &stm)) {
	if (asctime_r(&stm, str))
	    str[24] = 0;
	else
	    *str = 0;
    }
    else
	*str = 0;

}

void getCurrentTime(char* str)
{
    struct  tm* pTm;
    time_t secs =  time(NULL);
    pTm  = localtime(&secs);
    sprintf(str,"%d-%d-%d %d:%d:%d",pTm->tm_year+1900,pTm->tm_mon+1,pTm->tm_mday,pTm->tm_hour,pTm->tm_min,pTm->tm_sec);
    
}

void secondsToString(const time_t sec, char *str) {
    struct tm stm;
    if (localtime_r(&sec, &stm)) {
	if (asctime_r(&stm, str))
	    str[24] = 0;
	else
	    *str = 0;
    }
    else
	*str = 0;
} 
///////////////////--FUNCTION--///////////////////////////////////////////////////////////////////////////////
///Return current time (ms)
/// @param time 
///////////////////--NOITCNUF--////////////////////////////////////////////////////////////////////////////////
void secondTime(long& time)
{
    struct timeval start,end;
    long dif_time=0;
    gettimeofday(&start,NULL);
    long start_time = ((long)start.tv_sec)*1000 + (long)start.tv_usec/1000;
//    printf("\tstart time:%ld ms\n", start_time);
    time = start_time;
}

///////////////////--FUNCTION--///////////////////////////////////////////////////////////////////////////////
///
/// random data
/// @param num  
///
/// @return 
///////////////////--NOITCNUF--////////////////////////////////////////////////////////////////////////////////
float randomData(unsigned int num)
{
    srand ( currentTimeMillis() );
    return (rand() % num);
}

///////////////////--FUNCTION--///////////////////////////////////////////////////////////////////////////////
///
/// Current TIME (ms)
/// @return 
///////////////////--NOITCNUF--////////////////////////////////////////////////////////////////////////////////
uint32_t currentTimeUMillis()
{
    struct timeval tstruct;
    if (gettimeofday(&tstruct, NULL) < 0) {
        return 0;
    }
    return (((uint32_t) tstruct.tv_sec) * 1000) + tstruct.tv_usec;
    std::cout << (((uint32_t) tstruct.tv_sec) * 1000) + tstruct.tv_usec;
}
uint32_t currentTimeMillis()
{
    struct timeval tstruct;
    if (gettimeofday(&tstruct, NULL) < 0) {
        return 0;
    }
    return (((uint32_t) tstruct.tv_sec) * 1000) + tstruct.tv_usec;
    
    std::cout << (((uint32_t) tstruct.tv_sec) * 1000) + tstruct.tv_usec;
}


double getCurrentTimeBySec()
{
    struct timeval cpt;
	gettimeofday(&cpt, 0);
	return static_cast<double>(cpt.tv_sec) + (1e-6 * cpt.tv_usec);
}
char* replace(char* src, char* sub, char* dst)
{

    int pos =0;

    int offset =0;

    int srcLen, subLen, dstLen;

    char* pRet = NULL;

    srcLen = strlen(src);
    subLen = strlen(sub);
    dstLen = strlen(dst);
    pRet = (char*)malloc(srcLen + dstLen - subLen +1);
    {
        pos = strstr(src, sub) - src;
        memcpy(pRet, src, pos);
        offset += pos;
        memcpy(pRet + offset, dst, dstLen);
        offset += dstLen;
        memcpy(pRet + offset, src + pos + subLen, srcLen - pos - subLen);
        offset += srcLen - pos - subLen;
        *(pRet + offset) ='\0';
    }
    return pRet;
}
