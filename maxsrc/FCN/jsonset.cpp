//
// Created by max on 18.09.2020.
//
#include "jsonset.h"
#include <iostream>


void writejsonarray(std::ostream &s, float  *p, int nx) {
    s << "[";
    for (int i = 0; i < nx; i++) {
        //
        if (i) s << ",";
        s << p[i];
        }
    s << "]" << std::endl;
    }

bool readjsonarrays(std::istream &data, std::vector<std::vector<float>> &vect){

    std::vector<float> fv;
    while(data && readjsonarray(data, fv) ){
        vect.push_back(fv);
    }
    return true;
}

void mergeData::Print(std::ostream &s) {
    // std::cout << slidePosition << std::endl;

    for(long i=0; i<rowsCount+gap; i++) {

        s << "[";
        for(long j=0; j<rowSize; j++) {
            //
            if(j) s << ",";
            s << buf[i*rowSize+j];
        }
        s << "]" << std::endl;

    }
}


void rollingdata::Print(std::ostream &s) {

    long r0=std::max(0l, slidePosition-rowsCount);
    long r1=std::max(slidePosition, rowsCount);
   // std::cout << slidePosition << std::endl;

    for(long i=r0; i<r1; i++) {

        s << "[";
        for(long j=0; j<rowSize; j++) {
            //
            if(j) s << ",";
            s << buf[i*rowSize+j];
        }
        s << "]" << std::endl;

    }
}

template<class T> bool Treadjsonarray(std::istream &s, std::vector<T> &vect) {
    std::string v;
    int nx=vect.size();
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
        T f;
        int rtc=(s >> f).good();
        if(!rtc) {
            ok=false;
            break;
        }
        if(nx<=0) {
            vect.push_back(f);
        } else if (i<nx) {
            vect[i++]=f;
        } else {
            ok=false;
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
    return done & ok;
}

bool readjsonarray(std::istream &s, std::vector<float> &vect){
    return Treadjsonarray<float>(s,vect);
}

bool readjsonarray(std::istream &s, std::vector<int> &vect){
    return Treadjsonarray<int>(s,vect);
}


std::vector<float> readjsonarray(std::istream &s, int nx) {

    std::vector<float> vect(nx);
    Treadjsonarray<float>(s,vect);

    return vect;
}

mergeData mergeVectorRev(std::vector<std::vector<float>> &vect, int gap_) {
    mergeData data;
    int rows=vect.size();
    if(rows==0) return data;
    auto rowSize=vect[0].size();
    //long count=rows*rowsize;
    if(rowSize==0) return data;

    data.Allocate(rowSize, rows, gap_);

    float *pbuf=data.buf;

    for(int i=vect.size(); i--; ) {
      auto &src=vect[i];
      if(src.size()!=rowSize) {
          std::cerr << "Fatal size of data in row is broken" << src.size() << " "<< rowSize<< std::endl;
          return mergeData(); // broken
        }
      memcpy(pbuf, src.data(), sizeof(float)*rowSize);
      pbuf+=rowSize;
      }

    return data;
}

mergeData makeSparseVectorRev(std::vector<int> &vect, int size, int gap_) {
//
    mergeData data;
    data.Allocate(1, size, gap_);
//
    int *ovPtr=vect.data();
    int ovSize=vect.size();
//
    int iRow=0;
    for( int i = size; i--; iRow++) {

      if(ovSize && ovPtr[ovSize-1]>=i) {
         ovSize--;
         *data.Row( iRow )=1;
         }
      else {
         *data.Row( iRow )=0;
         }


      }
    return data;
}