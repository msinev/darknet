//
// Created by max on 18.09.2020.
//

#ifndef DARKNET_JSONSET_H
#define DARKNET_JSONSET_H
#include <cstring>
#include <vector>
#include <iostream>
#include <algorithm>

// Externally rolling dataset
// Slower but simpler than GPU


struct mergeData {
    float *buf;
    long rowSize;
    long rowsCount;
    int gap;

    ~mergeData() {
        if (buf)
            free(buf);
        }

    mergeData():buf(NULL), rowSize(0), rowsCount(0), gap(0) {    }
    void Allocate(int row, int rows, int gap_=0) {
        rowSize=row;
        rowsCount=rows;
        gap=gap_;
        buf=(float*)malloc((size_t)row*(rows+gap)*sizeof(float));
        if(gap!=0) memset(buf+(long)row*rows, 0, (size_t)gap*row*sizeof(float));
    }

    mergeData(mergeData &&r): buf(r.buf), rowSize(r.rowSize),  rowsCount(r.rowsCount), gap(r.gap)  {
        r.buf=NULL;
        r.rowsCount=0;
    }
    inline float *Row(int n) {
      return buf+rowSize*n;
      }

    void Print();

private:
    mergeData(const mergeData& other);  // must not copy huge data
    mergeData& operator=(const mergeData& other); // must not copy
};

struct rollingdata: protected mergeData {


    long slidePosition;

    ~rollingdata() {
        }

    void Append(float *row) {
        if(slidePosition==0) {
            memmove(&buf[rowsCount*rowSize], &buf[0], rowSize*gap*sizeof(float) );
            slidePosition=rowsCount;
            }
        memcpy(&buf[(--slidePosition)*rowSize], row, rowSize*sizeof(float) );
        }

    float *Data() { return &buf[slidePosition*rowSize]; }
//protected:
    void Allocate(int row, int rows, int slide=100) {  // Hiding supertype
        gap=slide;
        rowSize=row;
        rowsCount=rows;

        slidePosition=rows;
        if(gap!=0) memset(buf+(long)row*rows, 0, (size_t)gap*row*sizeof(float));
        buf=(float*)malloc((size_t)row*(rows+slide)*sizeof(float));
        }

    void Print();

};



bool readjsonarrays(std::istream &data, std::vector<std::vector<float>> &vect);

void writejsonarray(std::ostream &s, float  *p, int nx);

bool readjsonarray(std::istream &s, std::vector<float> &p);
bool readjsonarray(std::istream &s, std::vector<int> &vect);

std::vector<float> readjsonarray(std::istream &s, int nx);

inline void writejsonarray(std::ostream &s, std::vector<float> &p) {
    writejsonarray(s, p.data(), p.size());
  }

mergeData mergeVectorRev(std::vector<std::vector<float>> &vect, int gap=0);
mergeData makeSparseVectorRev(std::vector<int> &vect, int size, int gap=0);


#endif //DARKNET_JSONSET_H
