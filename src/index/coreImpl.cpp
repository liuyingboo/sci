#include <stdio.h>

#include "core.h"

int SoEyes::FitsIndex::initEnvir(const std::string home)
{
    this->genExtFitsHeaderTime = 0;
    this->genReadFitsTime = 0;
    this->genBuildIndexTime = 0;
    this->genWriteIndexTime = 0;
    this->genWriteIndexSideEffectTime = 0;    
    
//    string home = "/home/lyb/";

    this->workhome=home+"/fitsindex/";

    if(access(this->workhome.c_str(), F_OK) == -1)
    {
        if(mkdir(this->workhome.c_str(),0777))
        {
            std::cout << "Error occured!(mkdir-workhome)" << std::endl;
        }
    }

    this->onlinehome=home+"/fitsindex/onlineindex/";
    if(access(this->onlinehome.c_str(), F_OK) == -1)
    {
        if(mkdir(this->onlinehome.c_str(),0777))
        {
            std::cout << "Error occured!(mkdir-onlinehome)" << std::endl;
        }
    }

    this->offlinehome=home+"/fitsindex/offlineindex/";

    if(access(this->offlinehome.c_str(), F_OK) == -1)
    {
        if(mkdir(this->offlinehome.c_str(),0777))
        {
            std::cout << "Error occured!(mkdir-offlinehome)" << std::endl;
        }
    }

    this->genfitshome=home+"/fitsindex/fits/";
    if(access(this->genfitshome.c_str(), F_OK) == -1)
    {
        if(mkdir(this->genfitshome.c_str(),0777))
        {
            std::cout << "Error occured!(mkdir-genfitshome)" << std::endl;
        }
    }

    this->outfitshome = home+"/fitsindex/outfitshome/";
    if(access(this->outfitshome.c_str(), F_OK) == -1)
    {
        if(mkdir(this->outfitshome.c_str(),0777))
        {
            std::cout << "Error occured!(mkdir-genfitshome)" << std::endl;
        }
    }
}

///////////////////--FUNCTION--///////////////////////////////////////////////////////////////////////////////
///从文件读取文件然后在内存中生成索引信息,
/// @param fitsfile 待生成索引的文件
///////////////////--NOITCNUF--////////////////////////////////////////////////////////////////////////////////
void SoEyes::FitsIndex::offlineIndex(std::string fitsfile)
{
    ///输出路径
    uint32_t genStartTime, genEndTime;    
    const char* outdir = this->offlinehome.c_str(); // default to keep data in memory
    const char* dsn = "index";

    genStartTime = currentTimeMillis();
    std::auto_ptr<FITS> pInfile(new FITS(fitsfile, Read, true));
    genEndTime = currentTimeMillis();
    this->genReadFitsTime += (genEndTime - genStartTime);
    
    PHDU& image = pInfile->pHDU();
    
    genStartTime = currentTimeMillis();
    image.readAllKeys();    
    genEndTime = currentTimeMillis();
    this->genExtFitsHeaderTime += (genEndTime - genStartTime);

//    std::cout<< std::setw(40) << "OFFLINE INDEX BUILDING TIME:" << genEndTime -genStartTime << "(ms)" << std::endl;    

    //////////////////////////////////////////////////////
    ///开始进行索引构建
    genStartTime = currentTimeMillis();
    ibis::init();
    std::auto_ptr<ibis::tablex> ta(ibis::tablex::create());

    int i = 1;
    std::ostringstream valStr;

    for (std::map<String,Keyword*>::const_iterator ki = image.keyWord().begin(); ki != image.keyWord().end(); ki++){
        std::string tmp = (*ki).second->name();
        replaceStr(tmp,"-","_");
        
        switch ((*ki).second->keytype())
        {
            case Tint:
            {
                const KeyData<int>& thisKey = static_cast<const KeyData<int>&>(*(*ki).second);
                int val = thisKey.keyval();                
                ta->addColumn(tmp.c_str(), ibis::INT);
                if(i == 1)
                {
                    valStr << val;

                }else
                {
                    valStr << "," << val;                    
                }
            }
            break;
            case Tfloat:
            {
                const KeyData<float>& thisKey = static_cast<const KeyData<float>&>(*(*ki).second);
                float val = thisKey.keyval();
                ta->addColumn(tmp.c_str(), ibis::FLOAT);
                if(i == 1)
                {
                    valStr << val;

                }else
                {
                    valStr << "," << val;                    
                }                
            }
            break;
            case Tdouble:
            {
                const KeyData<double>& thisKey = static_cast<const KeyData<double>&>(*(*ki).second);
                double val = thisKey.keyval();
                ta->addColumn(tmp.c_str(), ibis::DOUBLE);
                if(i == 1)
                {
                    valStr << val;

                }else
                {
                    valStr << "," << val;
                    //   std::cout << valStr.str() << std:: endl;
                }                
            }
            break;
            case Tstring:
            {
                const KeyData<String>& thisKey = static_cast<const KeyData<String>&>(*(*ki).second);
                string val = thisKey.keyval();
                ta->addColumn(tmp.c_str(), ibis::TEXT);
                if(i == 1)
                {
                    valStr << val;

                }else
                {
                    valStr << ",\"" << val << "\"";                    
                }                
            }
            break;
            case Tlogical:
            {
                const KeyData<bool>& thisKey = static_cast<const KeyData<bool>&>(*(*ki).second);
                bool val = thisKey.keyval();
            }
            default:
                ta->addColumn(tmp.c_str(), ibis::TEXT);
                if(i == 1)
                {
                    valStr << "N/A";

                }else
                {
                    valStr << ",\"" << "N/A" << "\"";                    
                }                                
                break;
        }
        i++;
    }
    ta->appendRow(valStr.str().c_str());

    if (ta->mRows() > 0 && outdir != 0 && *outdir != 0) {
        unsigned int ierr = ta->write(outdir, dsn,"Hard Code");
        if (ierr < 0) {
            std::cout << "Error occured, Writing index!" << std::endl;
        }
    }
    genEndTime = currentTimeMillis();
    this->genWriteIndexTime += (genEndTime - genStartTime);        
    
    int build_indexes = 1;
    if (build_indexes > 0)
    { // build indexes
        std::auto_ptr<ibis::table> tbl(ibis::table::create(outdir));
        if (tbl.get() != 0)
        {
            genStartTime = currentTimeMillis();
            tbl->buildIndexes();
            genEndTime = currentTimeMillis();
            this->genBuildIndexTime += (genEndTime - genStartTime);            
        }
    }
    static unsigned int count = 0;
    std::cout << "Keyword:" << count++ << "Indexed!" << std::endl;    

}

void SoEyes::FitsIndex::offlineIndexCSV(std::string indexFolder)
{
    ///输出路径 
    const char* outdir = "/home/lyb/fitsindex/secindex"; // default to keep data in memory
    const char* dsn = "index";
    ///风隔符 
    const char* del = ", "; // delimiters
    int ierr, nrpf = 0;
    const char* indexing ;
    ///是否建立索引
    int build_indexes = 1;
    ///namestypes 建立索引的字符串 索引列:类型
    std::string namestypes,metatags;
    
    std::ostringstream oss;

    ///////////////////--FUNCTION--///////////////////////////////////////////////////////////////////////////////
    ///主要生成FASTBIT的查询命令
    /// @param buildString 
    /// @param sampleFile 
    ///
    /// @return 
    ///////////////////--NOITCNUF--////////////////////////////////////////////////////////////////////////////////
    
    std::string sampleFile="/home/lyb/testfits/spec-55859-F5902_sp16-051.fits";
    genCmdStr(namestypes,sampleFile);
    std::cout << namestypes << std::endl;

    for (int i = 0; i < 1; ++ i) {
        if (i > 0) oss << " ";
    }

    ibis::init();
    std::auto_ptr<ibis::tablex> ta(ibis::tablex::create());
    std::vector<const char*> csvfiles;
    csvfiles.push_back("/home/lyb/fitsindex/outfitshome/csvfile.txt");

    if (! namestypes.empty())
    {
        ta->parseNamesAndTypes(namestypes.c_str());
    }
    // process the CSV files
    for (size_t i = 0; i < csvfiles.size(); ++ i)
    {
        ierr = ta->readCSV(csvfiles[i], nrpf, outdir, del);
        if (ierr < 0)
        {
            std::cout << "Read CSV error" << std::endl;
        }
        else if (outdir != 0 && *outdir != 0)
        {
            ierr = ta->write(outdir, dsn, oss.str().c_str(), indexing, metatags.c_str());
            if (ierr < 0) {
                std::cout << "Write index error" << std::endl;
            }
            ta->clearData();
            if (build_indexes > 0)
            { // build indexes
                std::auto_ptr<ibis::table> tbl(ibis::table::create(outdir));
                if (tbl.get() != 0)
                {
                    tbl->buildIndexes();
                }
            }
        }//else if
    }//for
}
void SoEyes::FitsIndex::onlineIndex(std::string fitsfile)
{
    uint32_t genStartTime, genEndTime;    
///输出路径 
    const char* outdir = this->onlinehome.c_str();
    const char* dsn = "index";

    ibis::init();
    std::auto_ptr<ibis::tablex> ta(ibis::tablex::create());

    std::auto_ptr<FITS> pInfile(new FITS(fitsfile,Read,true));

    PHDU& image = pInfile->pHDU();

    image.readAllKeys();


    int i = 1;
    std::ostringstream valStr;
    genStartTime = currentTimeMillis();    
    for (std::map<String,Keyword*>::const_iterator ki = image.keyWord().begin(); ki != image.keyWord().end(); ki++){
        std::string tmp = (*ki).second->name();
        replaceStr(tmp,"-","_");
        
        switch ((*ki).second->keytype())
        {
            case Tint:
            {
                const KeyData<int>& thisKey = static_cast<const KeyData<int>&>(*(*ki).second);
                ta->addColumn(tmp.c_str(), ibis::INT);
                if(i == 1)
                {
                    valStr << thisKey.keyval();

                }else
                {
                    valStr << "," << thisKey.keyval();                    
                }
            }
            break;
            case Tfloat:
            {
                const KeyData<float>& thisKey = static_cast<const KeyData<float>&>(*(*ki).second);
                float val = thisKey.keyval();
                ta->addColumn(tmp.c_str(), ibis::FLOAT);
                if(i == 1)
                {
                    valStr << thisKey.keyval();

                }else
                {
                    valStr << "," << thisKey.keyval();                    
                }                
            }
            break;
            case Tdouble:
            {
                const KeyData<double>& thisKey = static_cast<const KeyData<double>&>(*(*ki).second);
                double val = thisKey.keyval();
                ta->addColumn(tmp.c_str(), ibis::DOUBLE);
                if(i == 1)
                {
                    valStr << thisKey.keyval();

                }else
                {
                    valStr << "," << thisKey.keyval();                    
                }                
            }
            break;
            case Tstring:
            {
                const KeyData<String>& thisKey = static_cast<const KeyData<String>&>(*(*ki).second);
                string val = thisKey.keyval();
                ta->addColumn(tmp.c_str(), ibis::TEXT);
                if(i == 1)
                {
                    valStr << thisKey.keyval();

                }else
                {
                    valStr << ",\"" << thisKey.keyval() << "\"";                    
                }                
            }
            break;
            case Tlogical:
            {
                const KeyData<bool>& thisKey = static_cast<const KeyData<bool>&>(*(*ki).second);
                bool val = thisKey.keyval();
            }
            default:
                ta->addColumn(tmp.c_str(), ibis::TEXT);
                if(i == 1)
                {
                    valStr << "N/A";

                }else
                {
                    valStr << ",\"" << "N/A" << "\"";                    
                }                                
                break;
        }
        i++;
    }
    ta->appendRow(valStr.str().c_str());
    if (ta->mRows() > 0 && outdir != 0 && *outdir != 0) {
        unsigned int ierr = ta->write(outdir, dsn,"Hard Code");
        if (ierr < 0) {
            std::cout << "Error occured, Writing index!" << std::endl;
        }
    }
    genEndTime = currentTimeMillis();

    this->genWriteIndexTime += (genEndTime - genStartTime);

    int build_indexes = 1;
    if (build_indexes > 0)
    { // build indexes
        std::auto_ptr<ibis::table> tbl(ibis::table::create(outdir));
        if (tbl.get() != 0)
        {
            genStartTime = currentTimeMillis();
            tbl->buildIndexes();
            genEndTime = currentTimeMillis();
            this->genBuildIndexTime += (genEndTime - genStartTime);            
        }
    }    
}
///////////////////--FUNCTION--///////////////////////////////////////////////////////////////////////////////
///生成FASTBit的命令字符串
/// @param buildStr
/// @param sampleFile 必须要和处理的文件在同一目录下
///////////////////--NOITCNUF--////////////////////////////////////////////////////////////////////////////////
void SoEyes::FitsIndex::genCmdStr(std::string& buildStr, std::string sampleFile)
{
    std::auto_ptr<FITS> pInfile(new FITS(sampleFile,Read,true));

    PHDU& image = pInfile->pHDU();

    image.readAllKeys();
    tmpKeyWordList.clear();
    for (std::map<String,Keyword*>::const_iterator ki = image.keyWord().begin(); ki != image.keyWord().end(); ki++){

        tmpKeyWordList.push_back((*ki).second); //Store KeyWord into a List container

    }//for
    
    std::list<Keyword*>::iterator itor;
    static int i=1;
    std::ostringstream oss;
    for(itor = tmpKeyWordList.begin(); itor != tmpKeyWordList.end();++itor) {
        std::string tmp;
        tmp = (*itor)->name();
        if(tmp.find('-') != std::string::npos)
        {
            replaceStr(tmp,"-","_");
            std::cout << tmp << std::endl;            
        }

        switch ((*itor)->keytype())
        {
            case Tint:
            {
                if(i == 1)
                {
                    oss << "\"F" << tmp << ":i";
                }
                else
                {
                    oss << ",F" << tmp << ":i";
                }
            }
            break;
            case Tfloat:
            {
                if(i == 1)
                {
                    oss << "\"F" << tmp << ":f";
                }
                else
                {
                    oss << ",F" << tmp << ":f";
                }
            }
            break;
            case Tdouble:
            {
                if(i == 1)
                {
                    oss << "\"F" << tmp << ":f";
                }
                else
                {
                    oss << ",F" << tmp << ":f";
                }
            }
            break;
            case Tstring:
            {
                if(i == 1)
                {
                    oss << "\"F" << tmp << ":t";
                }
                else
                {
                    oss << ",F" << tmp << ":t";
                }
            }
            break;
            case Tlogical:
            default:
                if(i == 1)
                {

                    oss << "\"F" << tmp << ":t";
                }
                else
                {
                    oss << ",F" << tmp << ":t";
                }
                break;
        }
        i ++;
    }//for
    buildStr = oss.str();
    tmpKeyWordList.clear();
}

int SoEyes::FitsIndex::readHeaders(std::list<string>& fullFitsPath)
{
    std::list<string>::iterator it;
    for(it = fullFitsPath.begin(); it != fullFitsPath.end(); it++ )
    {
        std::auto_ptr<FITS> pInfile(new FITS((*it).c_str(), Read,true));
        PHDU& image = pInfile->pHDU();

        image.readAllKeys();

        for (std::map<String,Keyword*>::const_iterator ki = image.keyWord().begin(); ki != image.keyWord().end(); ki++)
        {
            std::cout << *((*ki).second) << std::endl;
        }
    }

}

int SoEyes::FitsIndex::writeHeader(std::string fullFitsPath)
{

    char FLAGBEG[]="---SOEYES beg---";
    char FLAGEND[]="---SOEYES end---";
    char buff[16];
    FILE*  fin = fopen(fullFitsPath.c_str(), "r");
    if(fin != NULL)
    {
        int ierr  = fread(buff, 1, sizeof(buff), fin);
        fclose(fin);
        if(ierr > 0){
            if(strcmp(FLAGBEG,buff) == 0)
            {
                return 0;
            }
            else
            {
                fin = fopen(fullFitsPath.c_str(), "w");
                fwrite(FLAGBEG,sizeof(char),strlen(FLAGBEG),fin);
                fputs("\n",fin);
                fclose(fin);
            }
        }
    }

    FILE*  fout = fopen(fullFitsPath.c_str(), "a+");
    std::list<Keyword*>::iterator itor;
    std::cout << tmpKeyWordList.size()<< std::endl;
    for(itor = tmpKeyWordList.begin(); itor != tmpKeyWordList.end();++itor)
    {
        std::string rs((*itor)->name());
        std::string::size_type pos = rs.find("-");
        if (pos != std::string::npos)
        {
            rs.replace(pos, 1, "_");
        }
        fprintf(fout, "\t%s%s ", "T",rs.c_str());//Add a charactor "T" before any word
    }
    fputs("\n", fout);
    fputs(FLAGEND, fout);
    fputs("\n\n", fout);
    fclose(fout);
}
///////////////////--FUNCTION--///////////////////////////////////////////////////////////////////////////////
/// @deprecated
///
/// @param fullFitsPath
///
/// @return
///////////////////--NOITCNUF--////////////////////////////////////////////////////////////////////////////////
int SoEyes::FitsIndex::extraHeaderInfo(std::string fullFitsPath)
{
    std::auto_ptr<FITS> pInfile(new FITS(fullFitsPath,Read,true));

    PHDU& image = pInfile->pHDU();

    image.readAllKeys();

    for (std::map<String,Keyword*>::const_iterator ki = image.keyWord().begin(); ki != image.keyWord().end(); ki++){

        tmpKeyWordList.push_back((*ki).second); //Store KeyWord into a List container

    }//for
}
///////////////////--FUNCTION--///////////////////////////////////////////////////////////////////////////////
/// 抽取出文件的FITS头存入文件
/// @param fullFitsPath
/// @param fitsToWrite
///
/// @return
///////////////////--NOITCNUF--////////////////////////////////////////////////////////////////////////////////
int SoEyes::FitsIndex::extrFitsHeaderToFiles(std::string fullFitsPath,std::string fitsToWrite)
{
    const std::string fitsToRead = fullFitsPath;

    std::auto_ptr<FITS> pInfile(new FITS(fitsToRead, Read, true));

    PHDU& image = pInfile->pHDU();

    image.readAllKeys();

    for (std::map<String,Keyword*>::const_iterator ki = image.keyWord().begin(); ki != image.keyWord().end(); ki++){

        tmpKeyWordList.push_back((*ki).second); //Store KeyWord into a List container

    }//for

//    writeHeader(fitsToWrite);

    FILE*  fout = fopen(fitsToWrite.c_str(), "a+");
    std::list<Keyword*>::iterator itor;
    static int i=1;
    for(itor = tmpKeyWordList.begin(); itor != tmpKeyWordList.end();++itor) {

        //      std::cout << std::endl << "Name(P):" << (*itor)->name();
        switch ((*itor)->keytype())
        {
            case Tint:
            {
                const KeyData<int>& thisKey = static_cast<const KeyData<int>&>(**itor);
                int val = thisKey.keyval();

                //std::cout<< "pFits->pHDU().addKey(\""<<(*itor)->name()<<"\","<<val<<","<<"\"N/A\");"<<std::endl;

                if(i == 1)
                {
                    fprintf(fout, "%d", val);
                }else
                {
                    fprintf(fout, ", %d", val);
                }
            }
            break;
            case Tfloat:
            {
                const KeyData<float>& thisKey = static_cast<const KeyData<float>&>(**itor);
                float val = thisKey.keyval();
//                std::cout<< "pFits->pHDU().addKey(\""<<(*itor)->name()<<"\","<<val<<","<<"\"N/A\");"<<std::endl;
                if(i == 1)
                {
                    fprintf(fout, "%f", val);
                }
                else
                {
                    fprintf(fout, ", %f", val);
                }
            }
            break;
            case Tdouble:
            {
                const KeyData<double>& thisKey = static_cast<const KeyData<double>&>(**itor);
                double val = thisKey.keyval();
                //   std::cout<< "pFits->pHDU().addKey(\""<<(*itor)->name()<<"\","<<val<<","<<"\"N/A\");"<<std::endl;
                if(i == 1)
                {
                    fprintf(fout, "%f", val);
                }
                else
                {
                    fprintf(fout, ", %f", val);
                }
            }
            break;
            case Tstring:
            {
                const KeyData<String>& thisKey = static_cast<const KeyData<String>&>(**itor);
                string val = thisKey.keyval();
                //              std::cout<< "pFits->pHDU().addKey(\""<<(*itor)->name()<<"\",\""<<val<<"\","<<"\"N/A\");"<<std::endl;
                if(i == 1)
                {
                    fprintf(fout, "\"%s\"", val.c_str());
                }
                else
                {
                    fprintf(fout, ", \"%s\"", val.c_str());
                }
            }
            break;
            case Tlogical:
            {
                const KeyData<bool>& thisKey = static_cast<const KeyData<bool>&>(**itor);
                bool val = thisKey.keyval();
            }
            default:
                if(i == 1)
                {
                    std::string tmp;
                    tmp = (*itor)->name();
                    replaceStr(tmp,"-","_");
                    fprintf(fout, "\"%s\"", "N/A");
                    std::cout << "\"F" << tmp << ":t";
                }
                else
                {
                    std::string tmp;
                    tmp = (*itor)->name();
                    replaceStr(tmp,"-","_");
                    fprintf(fout, ", \"%s\"","N/A");
                    std::cout << ",F" << tmp << ":t";
                }
                break;
        }
        i ++;
    }//for

    fputs("\n\n",fout);

    fclose(fout);

    tmpKeyWordList.clear();
    i = 1;
    return 0;
}

int SoEyes::FitsIndex::writeFits(std::string fitsName, std::string val)
{

    // FILE*  fout = fopen(smpd_process.dbg_filename, "wb");
    // if (fout != NULL)
    // {

    //  fprintf(fout, " %d instead of %d bytes to the smpd log file.\n", num_written, new_size);
    // }
    // fclose(fout);

}
///////////////////--FUNCTION--///////////////////////////////////////////////////////////////////////////////
///从指定的文件夹目录下都取FITS文件头,并存入一个CSV文件下
/// @param fitsFolder
/// @param fullPath
///
/// @return
///////////////////--NOITCNUF--////////////////////////////////////////////////////////////////////////////////
int SoEyes::FitsIndex::readHeadersFromFitsFolder(std::string fitsFolder,std::list<string>& fullPath)
{

    int cnt = 1;
    DIR* dirp = opendir(fitsFolder.c_str());
    if (dirp == 0) return cnt;
    struct dirent* ent = 0;
    char fullNamePath[512];

    while ((ent = readdir(dirp)) != 0) {
        if ((ent->d_name[1] == 0 || ent->d_name[1] == '.') &&
            ent->d_name[0] == '.') { // skip '.' and '..'
            continue;
        }
        sprintf(fullNamePath, "%s%s%s", fitsFolder.c_str(),"/", ent->d_name);
        fullPath.push_back(fullNamePath);

    }
    closedir(dirp);
}

///////////////////--FUNCTION--///////////////////////////////////////////////////////////////////////////////
/// 模拟生成FITS文件功能
/// @param fileName
///
/// @return
///////////////////--NOITCNUF--////////////////////////////////////////////////////////////////////////////////
int SoEyes::FitsIndex::genSingleFitsFile(std::string fileName)
{
    
    uint32_t genStartTime, genEndTime,genFileStartTime,genFileEndTime;

    genStartTime = currentTimeMillis();
    genFileStartTime = genStartTime;
    
    long naxis    =   2;
    long naxes[2] = { 2560, 2160 };


    std::auto_ptr<FITS> pFits(0);

    try
    {
        pFits.reset( new FITS(this->genfitshome + fileName , USHORT_IMG , naxis , naxes ) );
    }
    catch (FITS::CantCreate)
    {
        return -1;
    }

    // references for clarity.

    long& vectorLength = naxes[0];
    long& numberOfRows = naxes[1];
    long nelements(1);

    nelements = std::accumulate(&naxes[0],&naxes[naxis],1,std::multiplies<long>());

    std::valarray<int> row(vectorLength);
    for (long j = 0; j < vectorLength; ++j) row[j] = j;
    std::valarray<int> array(nelements);
    for (int i = 0; i < numberOfRows; ++i)
    {
        array[std::slice(vectorLength*static_cast<int>(i),vectorLength,1)] = row + i;
    }

    const double PI ( std::atan(1.)*4. );
    // create some data for the image extension.
    long  fpixel(1);

    long exposure(1500);
    double re = std::cos( 2*PI/3.0 );
    double im = std::sin( 2*PI/3.0 );
    std::complex<float> omega( re, im );

    string keyws[]={"","AOSYS","CCDHEIGH","CCDWIDTH","COODANG","DATEOBS","EXPTIME","FOCALLEN","INSTRUME",
                    "OBSCOOD","OBSOBJ","OFFBAND","PASSBAND","TIMEOBS","WAVELEN","XBINING","XORGSUB","XPIXSZ",
                    "YBINNIG","YORGSUB","YPIXSZ"};

    float value[20] ={0,2560.0,2560.0,1,23,35000.0,5,6,7,8,34,34,34,34,34,2,1,1,6.5,6.5};    
    //value[1] = randomData(5)/1000;
    pFits->pHDU().addKey(keyws[1],value[1],"N/A");
    //value[2] = randomData(5000);
    pFits->pHDU().addKey(keyws[1],value[2],"N/A");
    //value[3] = randomData(5000);
    pFits->pHDU().addKey(keyws[3],value[3],"N/A");
    //value[4] = randomData(5);
    pFits->pHDU().addKey(keyws[4],value[4],"N/A");
    //value[5] = randomData(50);
    pFits->pHDU().addKey(keyws[5],value[5],"N/A");//TODO DATE_OBS
    //value[6] = randomData(50)/1000;
    pFits->pHDU().addKey(keyws[6],value[6],"N/A");
    //value[7] = randomData(50000);
    pFits->pHDU().addKey(keyws[7],value[7],"N/A");
    // value[8] = randomData(5);
    pFits->pHDU().addKey(keyws[8],value[8],"N/A");
    //value[9] = randomData(5);
    pFits->pHDU().addKey(keyws[9],value[9],"N/A");//TODO OBSCOOD
    // value[10] = randomData(5000);
    pFits->pHDU().addKey(keyws[10],value[10],"N/A");//TODO OBSOBJ
    // value[11] = randomData(5);
    pFits->pHDU().addKey(keyws[11],value[11],"N/A");
    // value[12] = randomData(50);
    pFits->pHDU().addKey(keyws[12],value[12],"N/A");
    //value[13] = randomData(50);
    pFits->pHDU().addKey(keyws[13],value[13],"N/A");//TODO TIME_OBS
    //value[14] = randomData(5000);
    pFits->pHDU().addKey(keyws[14],value[14],"N/A");
    // value[15] = randomData(5);
    pFits->pHDU().addKey(keyws[15],value[15],"N/A");
    //value[16] = randomData(5);
    pFits->pHDU().addKey(keyws[16],value[16],"N/A");
    //value[17] = randomData(900)/100;
    pFits->pHDU().addKey(keyws[17],value[17],"N/A");
    // value[18] = randomData(5);
    pFits->pHDU().addKey(keyws[18],value[18],"N/A");
    // value[19] = randomData(5);
    pFits->pHDU().addKey(keyws[19],value[19],"N/A");
    //value[20] = randomData(10)/10;
    pFits->pHDU().addKey(keyws[20],value[20],"N/A");

    pFits->pHDU().write(fpixel,nelements,array);


    // value[1] = randomData(5)/1000;//消耗CPU资院
    // value[2] = randomData(5000);
    // value[3] = randomData(5000);
    // value[4] = randomData(5);
    // value[5] = randomData(50);
    // value[6] = randomData(50)/1000;
    // value[7] = randomData(50000);
    // value[8] = randomData(5);
    // value[9] = randomData(5);
    // value[10] = randomData(5000);
    // value[11] = randomData(5);
    // value[12] = randomData(50);
    // value[13] = randomData(50);
    // value[14] = randomData(5000);
    // value[15] = randomData(5);
    // value[16] = randomData(5);
    // value[17] = randomData(900)/100;
    // value[18] = randomData(5);
    // value[19] = randomData(5);
    // value[20] = randomData(10)/10;

        

    // //////////////////////////////////////////////////////
    // ///开始进行索引构建
   
    // ///输出路径 
    // const char* outdir = this->onlinehome.c_str();
    // const char* dsn = "index";

    // genStartTime = currentTimeMillis();
    // ibis::init();
    // std::auto_ptr<ibis::tablex> ta(ibis::tablex::create());
    
    // int i = 1;
    
    // std::ostringstream valStr;
    // for (int j = 1; j <= 20; j++ )
    // {
    //     ta->addColumn(keyws[j].c_str(), ibis::FLOAT);
    //     if(j == 1)
    //     {
    //         valStr << value[j];

    //     }else
    //     {
    //         valStr << "," << value[j];                    
    //     }        
    // }
    // ta->appendRow(valStr.str().c_str());

    // if (ta->mRows() > 0 && outdir != 0 && *outdir != 0) {
    //     unsigned int ierr = ta->write(outdir, dsn,"Hard Code");
    //     if (ierr < 0) {
    //         std::cout << "Error occured, Writing index!" << std::endl;
    //     }
    // }
    // genEndTime = currentTimeMillis();
    // this->genWriteIndexTime += (genEndTime - genStartTime);                

    
    // int build_indexes = 1;
    // if (build_indexes > 0)
    // { // build indexes
    //     std::auto_ptr<ibis::table> tbl(ibis::table::create(outdir));
    //     if (tbl.get() != 0)
    //     {
    //         genStartTime = currentTimeMillis();
    //         tbl->buildIndexes();
    //         genEndTime = currentTimeMillis();
    //         this->genBuildIndexTime += (genEndTime - genStartTime);            
    //     }
    // }
    
    return 0;
}

void  SoEyes::FitsIndex::offlineIndexFolder(std::string fitsfolder)
{
    
    std::list<string> fitsFoldersList;
    readHeadersFromFitsFolder(fitsfolder,fitsFoldersList);
    std::list<string>::iterator itor;


    for(itor = fitsFoldersList.begin(); itor != fitsFoldersList.end();++itor)
    {
        offlineIndex(*itor);
    }

}
///////////////////--FUNCTION--///////////////////////////////////////////////////////////////////////////////
///生成随机的FITS文件
///////////////////--NOITCNUF--////////////////////////////////////////////////////////////////////////////////
void SoEyes::FitsIndex::genRandomFits(unsigned int numFits)
{
    char tstr[28];
    char fitsName[64];
    uint32_t ms = currentTimeMillis();
    this->genWriteIndexTime = 0;
    
    for(int j = 0 ; j < numFits; j++)
    {
        std::cout << "Building header index:"  << "  " << j <<  std::endl;
        this->genOnlineHeaderIndex();//把头提出来,单独测试
    }
    std::cout << "Building header index finished!"  << std::endl;
    
    for(int j = 0 ; j < numFits; j++)
    {
        getLocalTime(tstr);
        sprintf(fitsName,"%s-%ld-%d.fits",tstr,ms,j);
        int i=-1;
        while(fitsName[i++])
        {
            fitsName[i] = tolower(fitsName[i]);
        }
        std::string fname(fitsName);
        for (size_t pos = 0; (pos = fname.find(" ", pos)) != std::string::npos;pos += strlen(""))
        {
            fname.replace(pos, strlen(" "), "");
        }
        std::cout << "Saving Fits:" << fname << std::endl;
        this->genSingleFitsFile(fname);

    }

}
void SoEyes::FitsIndex::genOnlineHeaderIndex()
{

    uint32_t genStartTime, genEndTime,genFileStartTime,genFileEndTime;
    string keyws[]={"","AOSYS","CCDHEIGH","CCDWIDTH","COODANG","DATEOBS","EXPTIME","FOCALLEN","INSTRUME",
                    "OBSCOOD","OBSOBJ","OFFBAND","PASSBAND","TIMEOBS","WAVELEN","XBINING","XORGSUB","XPIXSZ",
                    "YBINNIG","YORGSUB","YPIXSZ"};

    float value[20] ={0,2560.0,2560.0,1,23,35000.0,5,6,7,8,34,34,34,34,34,2,1,1,6.5,6.5};
    value[1] = randomData(5)/1000;//消耗CPU资院
    value[2] = randomData(5000);
    value[3] = randomData(5000);
    value[4] = randomData(5);
    value[5] = randomData(50);
    value[6] = randomData(50)/1000;
    value[7] = randomData(50000);
    value[8] = randomData(5);
    value[9] = randomData(5);
    value[10] = randomData(5000);
    value[11] = randomData(5);
    value[12] = randomData(50);
    value[13] = randomData(50);
    value[14] = randomData(5000);
    value[15] = randomData(5);
    value[16] = randomData(5);
    value[17] = randomData(900)/100;
    value[18] = randomData(5);
    value[19] = randomData(5);
    value[20] = randomData(10)/10;

        

    //////////////////////////////////////////////////////
    ///开始进行索引构建
   
    ///输出路径 
    const char* outdir = this->onlinehome.c_str();
    const char* dsn = "index";

    genStartTime = currentTimeMillis();
    ibis::init();
    std::auto_ptr<ibis::tablex> ta(ibis::tablex::create());
    
    int i = 1;
    
    std::ostringstream valStr;
    for (int j = 1; j <= 20; j++ )
    {
        ta->addColumn(keyws[j].c_str(), ibis::FLOAT);
        if(j == 1)
        {
            valStr << value[j];

        }else
        {
            valStr << "," << value[j];                    
        }        
    }
    ta->appendRow(valStr.str().c_str());

    if (ta->mRows() > 0 && outdir != 0 && *outdir != 0) {
        unsigned int ierr = ta->write(outdir, dsn,"Hard Code");
        if (ierr < 0) {
            std::cout << "Error occured, Writing index!" << std::endl;
        }
    }
    genEndTime = currentTimeMillis();
    this->genWriteIndexTime += (genEndTime - genStartTime);                

    
    int build_indexes = 1;
    if (build_indexes > 0)
    { // build indexes
        std::auto_ptr<ibis::table> tbl(ibis::table::create(outdir));
        if (tbl.get() != 0)
        {
            genStartTime = currentTimeMillis();
            tbl->buildIndexes();
            genEndTime = currentTimeMillis();
            this->genBuildIndexTime += (genEndTime - genStartTime);            
        }
    }
}
