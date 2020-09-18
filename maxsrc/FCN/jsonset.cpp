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
