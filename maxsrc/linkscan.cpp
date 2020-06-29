//
// Created by max on 13.06.2020.
//

#include "shadeoftime.h"
//#include "opencv2/opencv.hpp"
#include <iostream>
#include <dirent.h>

using namespace std;


int main(int narg, char *sarg[]) {

    // Create a VideoCapture object and open the input file
    // If the input is the web camera, pass 0 instead of the video file name

    if (narg != 3) {
        cerr << "Error Invalid syntax\n" << sarg[0] << " DirIn DirOut" << endl;
        return -1;
    }

    DIR* dirp = opendir(sarg[1]);
    struct dirent * dp;

    while ((dp = readdir(dirp)) != NULL) {
        std::cout << dp->d_name << endl;
        }

    closedir(dirp);
}