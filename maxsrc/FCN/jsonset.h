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

struct rollingdata {

    float *buf;
    int rowSize;
    int rowsCount;
    int slideStep;
    int slidePosition;

    ~rollingdata() {
        if (buf)
            delete [] buf;
        }

    void Append(float *row) {
        if(slidePosition==slideStep+rowsCount) {
            memcpy(&buf[0], &buf[(slideStep+1)*rowSize], rowSize*(rowsCount-1)*sizeof(float) );
            slidePosition=rowsCount-1;
            }
        memcpy(&buf[(slidePosition++)*rowSize], row, rowSize*sizeof(float) );
        }
//protected:
    void Allocate(int row, int rows, int slide=100) {
        rowSize=row;
        rowsCount=rows;
        slideStep=slide;
        slidePosition=0;

        buf=new float[row*(rows+slide)];
        }

    void Print();

};

std::vector<float> readjsonarray(std::istream s, int nx);

#endif //DARKNET_JSONSET_H
