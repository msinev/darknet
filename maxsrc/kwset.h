//
// Created by max on 16.09.2020.
//

#ifndef DARKNET_KWSET_H
#define DARKNET_KWSET_H
#include <iostream>
#include <cstring>

struct classSet {
    const char **names;
    int classes;

    void freeBuf() {
        free(names);
        names=NULL;
        classes=0;

    }

    void print() {
        for(auto i=0; i<classes; i++) {
            std::cout << names[i] << std::endl;
        }

    }
    int readSet(std::string k);
};
#endif //DARKNET_KWSET_H
