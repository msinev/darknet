//
// Created by max on 19.09.2020.
//

#include "jsonset.h"
#include <iostream>
#include <fstream>
#include <string>


int main(int nargs, char *sarg[]) {
    //
    if(nargs!=3 && nargs!=2) {
        std::cout << "CMD [in.out] [threshold e.g .9]"  << std::endl;
        return 1;
        }

    double th=.9;
    std::ifstream datain(sarg[1]);

    if(!datain) {
        std::cout << sarg[1] << " not open!"  << std::endl;
        return 1;

    }

    std::vector<float> fv;
    if(nargs>2) {
        th=std::stod(std::string(sarg[2]));
        std::cout << "set threshold " << th << std::endl;
      }

    int N=0;
    int x=0;
    std::cout << "[";
    while(datain && readjsonarray(datain, fv) ){

      if(fv[0]>=th) {
        if (x==0)
            std::cout <<  N;
        else
          std::cout << "," << N ;
        x++;
        if(x%100==0) std::cout << std::endl;
        }
        //std::cout << N << " :" << fv[0] << " - " << fv[1] << std::endl;


        N++;
      }
    std::cout << "]" << std::endl;
    return 0;
}