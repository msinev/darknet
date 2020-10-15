//
// Created by max on 19.09.2020.
//
#pragma CXX11_ON
#include "jsonset.h"
#include <iostream>
/*
[0]
[1]
[0]
[1]
[0]
[1]
[1]
[0]
[0]
[1]
[0]
[0]
---
[4,1,2]
[3,1,2]
[3,1,2]
[1.1,1,2]
[0,0,0]
[0,0,0]
[0,0,0]
 */

int main() {
    //

    std::vector<int> v={0,3,4,6,8};
    auto s=makeSparseVectorRev(v, 10,2);
    s.Print(std::cout);
    std::cout << "---" <<  std::endl;
    //
    std::vector<float> s1={1.1,1.,2.};
    std::vector<float> s2={2.,1.,2.};
    std::vector<float> s3={3.,1.,2.};
    std::vector<float> s4={4.,1.,2.};
    //
    std::vector<std::vector<float>> ss={s1,s3,s3,s4};
    auto k=mergeVectorRev(ss,3);

    k.Print(std::cout);
}