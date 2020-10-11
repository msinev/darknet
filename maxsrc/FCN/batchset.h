//
// Created by max on 10.10.2020.
//

#ifndef DARKNET_BATCHSET_H
#define DARKNET_BATCHSET_H

#include <darknet.h>
#include <functional>
#include <cstring>
#include <vector>

class batchset: public data {
    const int batch,  learnRows;
    std::vector<float*> in, out;

public:
    batchset(int inpc, int outc, int btch=1, int K=1) : batch(btch), learnRows(btch*K), in(btch*K), out(btch*K)  {

        X.cols=inpc;
        X.rows=learnRows;
        X.vals=in.data();
        y.cols=outc;
        y.rows=learnRows;
        y.vals=out.data();
        }

    batchset(network *net, int K=1) : batch(net->batch), learnRows(net->batch*K), in(net->batch*K), out(net->batch*K)  {
        X.cols=net->inputs;
        X.rows=learnRows;
        X.vals=in.data();
        y.cols=net->outputs;
        y.rows=learnRows;
        y.vals=out.data();
        }

    bool datasetrows( std::function< bool(float *&, float *&) > const &row_lambda);
/*
 * Data not copied here - just pointer
 */
    void setdatasetrows(float *indata, float *outdata, int row) {
      in[row]=indata;
      out[row]=outdata;
      }

    int batchsize() {
        return batch;
    }

    int size() {
        return learnRows;
    }

};


#endif //DARKNET_BATCHSET_H
