//
// Created by max on 18.09.2020.
//

#ifndef DARKNET_JSONSET_H
#define DARKNET_JSONSET_H
#include <cstring>
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
protected:
    void Allocate(int row, int rows, int slide=100) {
        buf=new float[row*(rows+slide)];
        }

    void Append(float *row) {
        if(++slidePosition==slideStep) {
            memcpy(&buf[0], &buf[slideStep], rowSize*(rowsCount-1)*sizeof(float) );
            slidePosition=rowsCount-1;
        }
        memcpy(&buf[slidePosition*rowSize], row, rowSize*sizeof(float) );
    }
    void Print(float *row) {
        int r0=max(0, slidePosition-rowsCount); i
        for(int i=)
    }

};



#endif //DARKNET_JSONSET_H
