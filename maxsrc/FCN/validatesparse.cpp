//
// Created by max on 19.09.2020.
//

#include "jsonset.h"
#include <iostream>
#include <fstream>

int main(int nargs, char *sarg[]) {
    //
    if(nargs!=3) {
        std::cout << "CMD [in.sparse] [in.out]"  << std::endl;
        return 1;
        }
    std::ifstream datain(sarg[2]);
    std::ifstream indexin(sarg[1]);

    std::vector<int> v;
    std::vector<float> fv;
    if(!indexin || !readjsonarray(indexin, v) ) {
      std::cout << "Index error" << std::endl;
      return 2;
      }

    int N=0, i;
    i=0;
    while(datain && readjsonarray(datain, fv) ){
      N++;
      if(i>=v.size()) break;
      if(v[i]<N+5) {
        if  (v[i]==N) {

            std::cout << N << "* :" << fv[0] << " * " << fv[1] << std::endl;
        } else {
            std::cout << N << " :" << fv[0] << " - " << fv[1] << std::endl;

        }
        //std::cout << N << " :" << fv[0] << " - " << fv[1] << std::endl;
        }
      if(v[i]<N-5) {
          i++;

        }
      }

    return 0;
}