//
// Created by max on 18.09.2020.
//
#include "jsonset.h"
#include <iostream>

void rollingdata::Print() {

    int r0=std::max(0, slidePosition-rowsCount);
    int r1=std::max(slidePosition, rowsCount);
   // std::cout << slidePosition << std::endl;

    for(int i=r0; i<r1; i++) {

        std::cout << "[";
        for(int j=0; j<rowSize; j++) {
            //
            if(j) std::cout << ",";
            std::cout << buf[i*rowSize+j];
        }
        std::cout << "]" << std::endl;

    }
}

std::vector<float> readjsonarray(std::istream &s, int nx) {
    std::string v;
    std::vector<float> vect(nx);
    bool ok=true;
    bool done=false;

    while(ok)  {
        int c=s.peek();
        s.ignore() ;
        if (c=='[') break;
        if(c!=' ' && c!='\n' && c!='\t') ok=false;
    }
    int i=0;
    while(ok && !done) {
        float f;
        int rtc=(s >> f).good();
        if(!rtc) {
            ok=false;
            break;
          }
        if(nx<=0) {
            vect.push_back(f);
        } else {
            vect[i++]=f;
        }

        do {
            int c=s.peek();
            s.ignore() ;
            if (c==']') {
                done=true;
                break;
                }
            if(c==',') break;
            if(c!=' ' && c!='\n' && c!='\t') ok=false;
        }  while(ok);

        }
    return vect;
}