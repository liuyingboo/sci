#include <mysql/mysql.h>
#include <stdio.h>
#include "utils.h"
#include <pthread.h>
#include <sys/time.h>

#define MAX 10
pthread_t thread[2];
pthread_mutex_t mut;
int number = 0, i;

float rand_ma;
float rand_sr;
float rand_ra;
float rand_dec;
float rand_dateobs;

char tmpstr[100];
char sqlstr[256];
char times[30];
const char *server = "localhost";
const char *user = "root";
const char *password = "gsdjsj";
const char *database = "fits";
const char *insertSQL= "INSERT INTO fitshead(t_dateobs,t_ra,t_dec,t_sr,t_ma)";

struct thArg {
    const char* uid;
    unsigned int count;
    const std::vector<const char*>& qlist;
    thArg(const unsigned int NR, const char* id, const std::vector<const char*>& ql)
        : count(NR),uid(id), qlist(ql) {}
};

extern "C" void* thFun(void* arg) {
    thArg* myArg = (thArg*)arg; // recast the argument to the right type

    MYSQL conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    int result;

    mysql_init(&conn);

    if (!mysql_real_connect(&conn, server,
                            user, password, database, 0, NULL, 0)) {
        fprintf(stderr, "%s\n", mysql_error(&conn));
    }
    mysql_autocommit(&conn,0);

    memset(sqlstr,0,strlen(sqlstr));
    int count = 1;
    for(int i = 0; i < myArg->count; i++)
    {
        strcat(sqlstr,insertSQL);

        time_t secs =  time(NULL);
        rand_ra = randomData(359999999)/1000000;//0~360
        rand_dec = randomData(179999999)/1000000;//0~180
        rand_sr = randomData(499999)/100000;//0~5
        rand_ma = randomData(999999)/100000;//0~10


        strcat(sqlstr,"VALUES(");
        sprintf(tmpstr,"%ld",secs);
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

        printf("%s\n",sqlstr);

        mysql_query(&conn,sqlstr);
        if(count%100000 == 0)//commit
        {
            result = mysql_commit(&conn);
            if(!result)
            {
                printf("Inserted %u rows \n",count);
            }
            else
            {
                fprintf(stderr, "%s\n", mysql_error(&conn));
            }
        }

        memset(sqlstr,0,strlen(sqlstr));
        memset(tmpstr,0,strlen(tmpstr));
        memset(times,0,strlen(times));

        count++;
    }
    result = mysql_commit(&conn);
    if(!result)
    {
        printf("Inserted %u rows \n",count);
    }
    else
    {
        fprintf(stderr, "%s\n", mysql_error(&conn));
    }    
    mysql_close(&conn);
    return 0;
}

// printout the usage string
static void usage(const char* name) {
    std::cout << "usage:\n" << name << " [-n|N number of Record to write] "
              << std::endl;
} // usage


int main(int argc, char *argv[])
{

    unsigned int NR=0;
    for (int i=1; i<argc; ++i) {
        if (*argv[i] == '-') {
            switch (argv[i][1]) {
                case 'h':
                case 'H':
                    usage(*argv);
                    exit(0);
                case 'n':
                case 'N':
                    if (i+1 < argc && argv[i+1][0] != '-') {
                        ++ i;
                        NR = atoi(argv[i]);
                    }
                    break;
                case 'd':
                case 'D':

                    break;
                default:
                    usage(*argv);
                    exit(0);
            }
        }
        else { // assume to be a set of query conditioins
            usage(*argv);
            exit(0);
        }

    }
    if( NR == 0)
    {
        std::cout<< "Can not insert zero records into Database!"<< std::endl;
    }
    const char* uid = "LIUYINGBO-INDEX";
    const int nth = 1;

    std::vector<const char*> qlist;
    thArg args(NR, uid, qlist);

    std::vector<pthread_t> tid(nth);
    for (int i = 0; i < nth; ++ i)
    { //
        int ierr = pthread_create(&(tid[i]), 0, thFun, (void*)&args);
        if (ierr != 0)
        {
        }
    }
    for (int i = 0; i < nth; ++ i)
    { //
        pthread_join(tid[i],NULL);
    }

    return 0;
}
