#include <stdio.h>

#include "core.h"

using namespace SoEyes;

int main(int argc, char *argv[])
{

//    FITS::setVerboseMode(true);
    char* home = 0;
    for (int i=1; i<argc; ++i) {
        if (*argv[i] == '-') { //colorder normal arguments starting with -
            switch (argv[i][1]) {
                case 'h':
                case 'H':
                    exit(0);
                case 'd':                     
                case 'D': 
                    if (i+1 < argc && argv[i+1][0] != '-') {
                        ++ i;
                        home = argv[i];
                    }
                    break;                    
                default:
                    std::cout << "Wrong arguments!" << std::endl;
                    exit(0);                    
            }
        }
        else { // assume to be a set of query conditioins
            std::cout << "Wrong arguments!" << std::endl;
            exit(0);
        }

    }

    if(home == NULL) exit(0);
    FitsIndex index;
    index.initEnvir(string(home));
    
///////////////////--FUNCTION--///////////////////////////////////////////////////////////////////////////////
///
///examples/ibis -d test -q "select FAZ where F1D_Z-0.000202>=-0.000001 && F1D_Z-0.000202<=0.000001"
/// @return 
///////////////////--NOITCNUF--////////////////////////////////////////////////////////////////////////////////


    ///要索引的目录-不要有子目录
//    std::string fitsFolder = "/home/lyb/testfits/spec-55859-F5902_sp16-051.fits";
    
    try
    {
//        index.offlineIndex(fitsFolder);
        index.offlineIndexFolder(index.genfitshome);
        std::cout<< std::setw(40) << "OFFLINE Index BUILDING TIME:" << index.genBuildIndexTime << "(ms)" << std::endl;
        std::cout<< std::setw(40) << "OFFLINE Extra FITS HEADER TIME:" << index.genExtFitsHeaderTime << "(ms)" << std::endl;
        std::cout<< std::setw(40) << "OFFLINE Write Index HEADER TIME:" << index.genWriteIndexTime << "(ms)" << std::endl;
        std::cout<< std::setw(40) << "OFFLINE Read FITS TIME:" << index.genReadFitsTime << "(ms)" << std::endl;            

    }
    catch (FitsException&)
    {
        std::cerr << " Fits Exception Thrown by test function \n";
    }
    return 0;
}
