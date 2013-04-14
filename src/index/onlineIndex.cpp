#include <stdio.h>

#include "core.h"

using namespace SoEyes;
// printout the usage string
static void usage(const char* name) {
    std::cout << "usage:\n" << name << " [-n|N number of fits to write] "
        "[-d directory-to-write-data]\n\n"
        "Note:\n\t for example: ./onlineindex -d /home/lyb"

              << std::endl;
} // usage

int main(int argc, char *argv[])
{

//    FITS::setVerboseMode(true);
    unsigned int numberOfFits = 0;
    char* home = 0;
    for (int i=1; i<argc; ++i) {
        if (*argv[i] == '-') { //colorder normal arguments starting with -
            switch (argv[i][1]) {
                case 'h':
                case 'H':
                    usage(*argv);
                    exit(0);
                case 'n':
                case 'N':
                    if (i+1 < argc && argv[i+1][0] != '-') {
                        ++ i;
                        numberOfFits = atoi(argv[i]);
                    }
                    break;
                case 'd':
                case 'D':
                    if (i+1 < argc && argv[i+1][0] != '-') {
                        ++ i;
                        home = argv[i];
                    }
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
    if(home == NULL)
    {
        usage(*argv);
        exit(0);
    }
    FitsIndex index;
    index.initEnvir(string(home));

    try
    {
        index.genRandomFits(numberOfFits);
        ///在线索引构建只需要监控索引构建和索引写入
        std::cout<< std::setw(40) << "ONLINE Index BUILDING TIME:" << index.genBuildIndexTime << "(ms)" << std::endl;
        std::cout<< std::setw(40) << "ONLINE Write Index HEADER TIME:" << index.genWriteIndexTime  << "(ms)" << std::endl;

    }
    catch (FitsException&)
    {
        std::cerr << " Fits Exception Thrown by test function \n";
    }
    return 0;
}
