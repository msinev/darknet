//
// Created by max on 10.10.2020.
//

#include "batchset.h"
bool batchset::datasetrows(std::function<bool (float *&, float *&)> const &row_lambda) {

    for (int i=0; i<learnRows; i++) {
        if (!row_lambda(in[i], out[i])) return false;
    }
    return true;
}

