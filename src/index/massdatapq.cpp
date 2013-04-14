#include "utils.h"
#include <stdio.h>

#include <stdlib.h>
#include <postgresql/libpq-fe.h>

char tmpstr[100];
char sqlstr[256];
char times[30];

float rand_ma;
float rand_sr;
float rand_ra;
float rand_dec;
float rand_dateobs;

const char *insertSQL= "INSERT INTO fitsheader(t_dateobs,t_ra,t_dec,t_sr,t_ma)";

static void exit_nicely(PGconn *conn) 

{ 

    PQfinish(conn); 

    exit(1); 

} 
int main() 
{
    int i, nFields, j; 
    double startTime,endTime;
    struct pg_conn * conn = 0; 

    PGresult *res = 0; 

    conn = PQsetdbLogin("127.0.0.1", "5432", "", "", "fits", "lyb", "gsdjsj"); 

    if(PQstatus(conn) != CONNECTION_OK) 
    { 
        printf("connect fail n"); 
    }else 
    { 
        printf("connect successn"); 
    }
#define USETRANS    
#ifdef USETRANS
    std::cout << "using transtraction" << std::endl;
    /* start a transaction block */
    res = PQexec(conn, "BEGIN"); 
    if(PQresultStatus(res) != PGRES_COMMAND_OK) 
    { 
        printf("execute sql fail %s", PQerrorMessage(conn)); 
        PQclear(res); 
        exit_nicely(conn); 
    } 
    PQclear(res);
#endif
    memset(sqlstr,0,strlen(sqlstr));
    int count = 0;
    startTime = getCurrentTimeBySec();
    for(int i = 0; i < 200000000; i++)
    {
        strcat(sqlstr,insertSQL);

        time_t secs =  time(NULL);

        rand_dateobs = static_cast<float>(secs);
        rand_ra = randomData(359999999)/1000000;//0~360
        rand_dec = randomData(179999999)/1000000;//0~180
        rand_sr = randomData(499999)/100000;//0~5
        rand_ma = randomData(999999)/100000;//0~10        

        
        strcat(sqlstr,"VALUES(");
        sprintf(tmpstr,"%f",rand_dateobs);
        strcat(sqlstr,tmpstr);
        strcat(sqlstr,",");

        sprintf(tmpstr,"%f",rand_ra);
        strcat(sqlstr,tmpstr);
        strcat(sqlstr,",");

        sprintf(tmpstr,"%f",rand_dec);
        strcat(sqlstr,tmpstr);
        strcat(sqlstr,",");

        sprintf(tmpstr,"%f",rand_sr);
        strcat(sqlstr,tmpstr);
        strcat(sqlstr,",");

        sprintf(tmpstr,"%f",rand_ma);
        strcat(sqlstr,tmpstr);
        strcat(sqlstr,")");

#ifdef USETRANS                        
        printf("%s\n",sqlstr);
#endif
        PQexec(conn, sqlstr); 
#ifdef USETRANS                
        if(count%10000 == 0)//commit
        {
            /* commit the transaction */
            res = PQexec(conn, "COMMIT");
            PQclear(res);

            /* start a transaction block */
            res = PQexec(conn, "BEGIN"); 
            if(PQresultStatus(res) != PGRES_COMMAND_OK) 
            { 
                printf("execute sql fail %s", PQerrorMessage(conn)); 
                PQclear(res); 
                exit_nicely(conn); 
            } 
            PQclear(res);            
        }
#endif
        memset(sqlstr,0,strlen(sqlstr));
        memset(tmpstr,0,strlen(tmpstr));
        memset(times,0,strlen(times));

        count++;
    }

    endTime = getCurrentTimeBySec();
    std::cout << "Total time " << endTime - startTime << "s(" << count << ")" << std::endl;
    PQfinish(conn); 
    return 0; 

} 


