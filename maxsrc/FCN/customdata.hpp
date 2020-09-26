//
// Created by max on 21.03.19.
//

#ifndef CUSTOM_DATASET_HPP
#define CUSTOM_DATASET_HPP
//#define OPENCV 1
#define GPU 1
#include <iostream>
#include <vector>
#include <list>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/process/environment.hpp>
#include <boost/foreach.hpp>
#include "darknet.h"
#ifdef OPENCVFIX
#include <math.h>
static inline int cvRound(float value)
{
    return (int)roundf(value);
}
#endif

#include <iostream>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>

class dataBuilder {
public:
    virtual data buildData()=0;
};

class imageSource {
public:
    virtual image getImage()=0;
};

struct networkImageInput {
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual int getChannels() const { return 3; }
};

struct networkSquareColorImageInput: networkImageInput {
    int size;
    networkSquareColorImageInput(int i) : size(i) {   }
    virtual int getWidth() const override   {
        return size;
    }
    virtual int getHeight() const override {
        return size;
    }
};

struct networkSizeImageInput: networkImageInput {
    int w, h, c;
    networkSizeImageInput(int iw, int ih, int ic=3) : w(iw), h(ih) {   }
    networkSizeImageInput(network *n) : w(n->w), h(n->w), c(n->c) {   }
    virtual int getWidth() const override {
        return w;
    }
    virtual int getHeight() const override {
        return h;
    }
    virtual int getChannels() const override {
        return c;
    }
};


struct imgParams {
    int min;
    int max;
    int size;
    float angle;
    float aspect;
    float hue;
    float saturation;
    float exposure;
    int center;
};

struct verticalVector: matrix  {
    verticalVector(int Xn) { // Vector
        rows = Xn;
        vals = (float **)calloc(rows, sizeof(float *));
        cols = 0;
    }
};


bool checkFilePath(boost::filesystem::path& abs, boost::filesystem::path name,  std::list<boost::filesystem::path> &path) {
//
    if(boost::filesystem::exists(name) && boost::filesystem::is_regular_file(name)) {
        abs=name;
        return true;
    }
//
    for(auto it = path.begin(); it != path.end(); ++it){
        boost::filesystem::path newpath=*it / name;
        std::cout << newpath << " checking..." << std::endl;
        if(boost::filesystem::exists(newpath) && boost::filesystem::is_regular_file(newpath)) {
            abs=newpath;
            std::cout << newpath << " found!" << std::endl;
            return true;
        }
    }
//
    return false;
}

bool checkDirPath(boost::filesystem::path& abs, boost::filesystem::path name,  std::list<boost::filesystem::path> &path) {
//
    if(boost::filesystem::exists(name) && boost::filesystem::is_regular_file(name)) {
        abs=name;
        return true;
    }
//
    for(auto it = path.begin(); it != path.end(); ++it){
        boost::filesystem::path newpath=*it / name;
        std::cout << newpath << " checking..." << std::endl;
        if(boost::filesystem::exists(newpath) && boost::filesystem::is_directory(newpath)) {
            abs=newpath;
            std::cout << newpath << " found!" << std::endl;
            return true;
        }
    }
//
    return false;
}


/*

class dirRandomLabelDataBuilder: public dataBuilder {
    const std::vector<std::vector<boost::filesystem::path>> imgs;
    const std::vector<std::string> labels;
    const networkImageInput &netParam;
    int batch;
public:
    dirRandomLabelDataBuilder(const  std::vector<std::vector<boost::filesystem::path>> &im,
                              const  std::vector<std::string> &l, const networkImageInput &p, int batchSize=32):
            imgs(im), labels(l), batch(batchSize), netParam(p) { }


    void setBatch(int b) {
        batch=b;
    }

    data buildData() override {
        data d={0, 0};

        int nLabels=imgs.size();


        d.shallow = 0;
        d.w=netParam.getWidth();
        d.h=netParam.getHeight();
        d.X=verticalVector(batch);
        d.y=make_matrix(batch, nLabels);

        for(int i=0; i<batch; i++) {
// choose random image
            int iLabel=rand()%imgs.size();
            int iImage=rand()%imgs[iLabel].size();
            auto path=imgs[iLabel][iImage];

// load choosen image
            imageCropLoader loader(path, netParam);
            d.X.vals[i]=loader.getImage().data;
// set label's Neuron to 1.0 (and rest is 0) - target output
            auto targetVector=d.y.vals[i];
            memset(targetVector, 0, sizeof(targetVector[0])*nLabels);
            //std::cerr << iLabel << " ";
            targetVector[iLabel]=1.0;
        }

        int i;

        return d;
*/
/*
     // label paths (char **paths, int n, char **labels, int k, tree *hierarchy)

        if(m) paths = me_get_random_paths(paths, n, m);
        data d = {0};
        d.shallow = 0;
        d.w=size;
        d.h=size;
// !!
        d.X = me_load_image_augment_paths(paths, n, min, max, size, angle, aspect, hue, saturation, exposure, center);
        d.y = me_load_labels_paths(paths, n, labels, k, hierarchy);
        data d={0, 0};

        return d;
*//*
    }

};
*/

#endif //CLASSIFY_IMAGES_HPP
