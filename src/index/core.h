#ifndef COMMON_H
#define COMMON_H

#include <CCfits/CCfits>
#include <CCfits/Image.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <cmath>
#include <fitsio.h>
#include <dirent.h>
#include <vector>
#include <sstream>



#include <cctype>
#include <algorithm>

#include <ibis.h>

#include "utils.h"

using namespace CCfits;
using std::valarray;


namespace SoEyes
{

     class FitsIndex
     {
     public:
          ///Working places
          std::string workhome;
          ///real time index home
          std::string onlinehome;
          ///secondly scaning stored fits file home
          std::string offlinehome;
          ///random fits file  home
          std::string genfitshome;
          ///output fits
          std::string outfitshome;


	 public:
		  uint64_t genReadFitsTime;

		  uint64_t genExtFitsHeaderTime;

		  uint64_t genBuildIndexTime;

		  uint64_t genWriteIndexTime;

		  uint64_t genWriteIndexSideEffectTime;		  


     public:

          std::list<Keyword*> tmpKeyWordList;
     public:
          int initEnvir();
          int initEnvir(const std::string home);		  
          int genSingleFitsFile(std::string fileName);
          int readHeadersFromFitsFolder(std::string fitsFolder, std::list<string>& fullPath);

          int readHeaders(std::list<string>& fullFitsPath);
          int writeFits(std::string fitsName, std::string val);

          int writeHeader(std::string fullFitsPath);
          int extraHeaderInfo(std::string fullFitsPath);
          int extrFitsHeaderToFiles(std::string fullFitsPath,std::string fitsToWrite);

          void genRandomFits(unsigned int numFits);
     public:
          void genCmdStr(std::string& buildStr, std::string sampleFile);
///////////////////--FUNCTION--///////////////////////////////////////////////////////////////////////////////
/// �Ȱ��ļ����µ�FITS�ļ�����CSV�ļ��� ,�ڽ�����������
/// @param indexFolder 
///////////////////--NOITCNUF--////////////////////////////////////////////////////////////////////////////////
          void offlineIndexCSV(std::string indexFolder);
		  
///////////////////--FUNCTION--///////////////////////////////////////////////////////////////////////////////
///�Ȱ��ļ����µ�FITS�ļ������ڴ���
/// @param fitsfile 
///////////////////--NOITCNUF--////////////////////////////////////////////////////////////////////////////////		  
          void offlineIndex(std::string fitsfile);
		  
///////////////////--FUNCTION--///////////////////////////////////////////////////////////////////////////////
/// ���߶����ڴ潨������
/// @param indexFolder 
///////////////////--NOITCNUF--////////////////////////////////////////////////////////////////////////////////		  		  
          void onlineIndex(std::string fitsfile);
///////////////////--FUNCTION--///////////////////////////////////////////////////////////////////////////////
///�����ļ����µ�FITS�ļ�,
/// @param fitsfolder ���������ļ���
///////////////////--NOITCNUF--////////////////////////////////////////////////////////////////////////////////
		  void offlineIndexFolder(std::string fitsfolder);

///////////////////--FUNCTION--///////////////////////////////////////////////////////////////////////////////
/// ��Ҫģ��FITSͷ����������,ԭ������Fitsͷ�������޹�
///////////////////--NOITCNUF--////////////////////////////////////////////////////////////////////////////////
		  void indexOnlineHeader();
			   
		  void genOnlineHeaderIndex();		  
     };
	 

}// namespace SoEyes
#endif
