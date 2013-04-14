#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>
using namespace std;


int main(int argc, char *argv[])
{
    time_t start;
    struct  tm* p ;

    time( &start );
    p = localtime(&start)    ;

    start = mktime(p);
    
    char str[28];
    
    getLocalTime(str);
    printf("%s\n", str);
    printf( "%d\n", start);
    printf( "%s\n", ctime(&start));
    getCurrentTime(str);
    printf( "%s\n",str);
    return 0;
}






