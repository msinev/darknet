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
        buf=new float[row*rows*slide];
        }

    void Append(float *row) {
        memcpy(sizeof(float))
    }
};



#endif //DARKNET_JSONSET_H
