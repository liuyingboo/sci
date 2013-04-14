#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <cmath>
#include <fitsio.h>
#include <dirent.h>
#include <vector>
#include <sstream>

#include <time.h>
#include <sys/time.h>
#include <string.h>

#define uint64_t unsigned long long
#define uint32_t unsigned long

uint32_t currentTimeMillis();
uint32_t currentTimeUMillis();
float randomData(unsigned int num);
char* replace(char* src, char* sub, char* dst);
int getSystemTime();
void secondTime();
void getLocalTime(char *str);
void getCurrentTime(char* str);
void secondsToString(const time_t sec, char *str);
void replaceStr(std::string& str, const std::string ori,const std::string ne);
double getCurrentTimeBySec();
#endif


















