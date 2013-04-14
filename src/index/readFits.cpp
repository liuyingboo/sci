#include <stdio.h>
#include "core.h"
using namespace CCfits;
using std::valarray;

int readImage(std::string fullFitsPath);
std::list<Keyword*> tmpKeyWordList;

int main(int argc, char *argv[])
{
	 FITS::setVerboseMode(true);
     try                     
     {
		  readImage("");

     }
     catch (FitsException&) 
     {
             
		  std::cerr << " Fits Exception Thrown by test function \n";       
             
     }
	 return 0;

}



int writeHeader(std::string fullFitsPath)
{
   
    FILE*  fout = fopen(fullFitsPath.c_str(), "a+");
    std::list<Keyword*>::iterator itor;
    for(itor = tmpKeyWordList.begin(); itor != tmpKeyWordList.end();++itor)
    {
        fprintf(fout, "\t%s ", (*itor)->name().c_str());
    }
    fputs("\n\n", fout);
    fclose(fout);    
}
///////////////////--FUNCTION--///////////////////////////////////////////////////////////////////////////////
///
/// Extrieve the information of a fits file, and store the header of a fits into a list container with a wrapper
/// std::list, It is because that I am not sure about std::map has consisit with each time to read?
/// @param fullFitsPath 
///
/// @return 
///////////////////--NOITCNUF--////////////////////////////////////////////////////////////////////////////////
int extraHeaderInfo(std::string fullFitsPath)
{
    std::auto_ptr<FITS> pInfile(new FITS(fullFitsPath,Read,true));
        
    PHDU& image = pInfile->pHDU(); 
        
    image.readAllKeys();

    for (std::map<String,Keyword*>::const_iterator ki = image.keyWord().begin(); ki != image.keyWord().end(); ki++){

        tmpKeyWordList.push_back((*ki).second); //Store KeyWord into a List container
        
    }//for
    
}

int readImage(std::string fullFitsPath)
{
    const std::string fitsToRead = "/home/lyb/projects/data/nvst.fits";
    std::string fitsToWrite = "test0000000000000t.fits";

    std::auto_ptr<FITS> pInfile(new FITS(fitsToRead, Read, true));
        
    PHDU& image = pInfile->pHDU(); 
        
    image.readAllKeys();

    for (std::map<String,Keyword*>::const_iterator ki = image.keyWord().begin(); ki != image.keyWord().end(); ki++){

        tmpKeyWordList.push_back((*ki).second); //Store KeyWord into a List container
        
    }//for
    
    writeHeader(fitsToWrite);

    FILE*  fout = fopen(fitsToWrite.c_str(), "a+");
    std::list<Keyword*>::iterator itor;    
    for(itor = tmpKeyWordList.begin(); itor != tmpKeyWordList.end();++itor)
    {
//        std::cout << "Name(P):" << (*itor)->name() << std::endl;
//        std::cout << "Type(T):" << (*itor)->keytype() << std::endl;//

        switch ((*itor)->keytype())
        {
            case Tint:
            {
                const KeyData<int>& thisKey = static_cast<const KeyData<int>&>(**itor);
                int val = thisKey.keyval();
                std::cout<< "pFits->pHDU().addKey(\"" << (*itor)->name() << "\",randomData(500000)/1000," << val << ");" << std::endl;
                //fprintf(fout, "\t%d ", val);
            }
            break;
            case Tfloat:
            {
                const KeyData<float>& thisKey = static_cast<const KeyData<float>&>(**itor);
                float val = thisKey.keyval();
                std::cout<< "pFits->pHDU().addKey(\"" << (*itor)->name() << "\",randomData(500000)/1000," << val << ");" << std::endl;                
                //fprintf(fout, "\t%f ", val);
            }
            break;
            case Tdouble:
            {
                const KeyData<double>& thisKey = static_cast<const KeyData<double>&>(**itor);
                double val = thisKey.keyval();
                std::cout<< "pFits->pHDU().addKey(\"" << (*itor)->name() << "\",randomData(500000)/1000," << val << ");" << std::endl;                
                //fprintf(fout, "\t%f ", val);                
            }
            break;
            case Tstring:
            {
                const KeyData<String>& thisKey = static_cast<const KeyData<String>&>(**itor); 
                string val = thisKey.keyval();
                std::cout<< "pFits->pHDU().addKey(\"" << (*itor)->name() << "\",randomData(500000)/1000," << val << ");" << std::endl;                
                //fprintf(fout, "\t%s", val.c_str());
            }
            break; 
            default:
                fprintf(fout, "\t%s", "N/A");                
                break;
        }
        
    }//for

    fputs("\n\n",fout);

    fclose(fout);

    return 0;   
}
