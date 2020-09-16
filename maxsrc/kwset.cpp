//
// Created by max on 16.09.2020.
//
#include "kwset.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

std::string trim(const std::string& str, char c)
{
    size_t first = str.find_first_not_of(c);
    if (std::string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(c);
    return str.substr(first, (last - first + 1));
}


int classSet::readSet(std::string k ) {
    std::ifstream inmap(k);

    if(!inmap) {
        names=NULL;
        classes=0;
        return classes;
    }


    std::vector<std::string> vline;
    int countbuf=0;


    for(std::string fline; std::getline(inmap, fline);) {
        std::string line=trim(trim(fline, ' '), '\n');
        if(!line.empty() && line.substr(0,1)!="#") {
            vline.push_back(line);
            countbuf += line.length();
        }
    }

    if(vline.empty()) {
        names=NULL;
        classes=0;
        return classes;
    }

    int bufSize=vline.size()*(sizeof(char*)+2)+countbuf;
    char *buff=(char*)malloc(bufSize);
    char *lbuff=buff+vline.size()*sizeof(char*);
    char **dptr=(char**)buff;
    names=(const char **)dptr;
    classes=vline.size();
    for(std::vector<std::string>::iterator it = vline.begin(); it != vline.end(); ++it) {
        *dptr++=lbuff;
        std::string &i=*it;
        memcpy(lbuff, i.data(), i.size());
        lbuff+=i.size();
        *lbuff++='\0';
    }

    return classes;
}

