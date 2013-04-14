#include <stdio.h>

#include "core.h"

int main(int argc, char *argv[])
{

    FITS::setVerboseMode(true);

    SoEyes::FitsIndex index;
    index.initEnvir("");

    std::string fitsToWrite = index.outfitshome + "/csvfile.csv";
    std::stringstream sstr;

    string fitsFolder = "/home/lyb/testfits";
    std::list<string> fitsFiles;
    try
    {

        index.readHeadersFromFitsFolder(fitsFolder,fitsFiles);
        std::list<string>::iterator itor;
        for(itor = fitsFiles.begin(); itor != fitsFiles.end();++itor)
        {
            std::cout << "Fits Folders:" << *itor << std::endl;
            index.extrFitsHeaderToFiles(*itor, fitsToWrite);
        }
    }
    catch (FitsException&)
    {
        std::cerr << " Fits Exception Thrown by test function \n";
    }
    return 0;
}
