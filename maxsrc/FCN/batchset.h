//
// Created by max on 10.10.2020.
//

#ifndef DARKNET_BATCHSET_H
#define DARKNET_BATCHSET_H

#include <darknet.h>

class batchset: data {
    int batch;
    float *in, *out;

};


#endif //DARKNET_BATCHSET_H
