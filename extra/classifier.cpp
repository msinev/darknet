//
// Created by max on 23.05.2020.
//

#include "classifier.h"
#include "darknet.h"
#include <iostream>
#include "network.h"
#include "utils.h"
#include "parser.h"
#include "option_list.h"
#include "blas.h"
#include "assert.h"
#include "classifier.h"
#include "dark_cuda.h"
#ifdef WIN32
#include <time.h>
#include "gettimeofday.h"
#else
#include <sys/time.h>
#endif
int main(int nagrg, char **sarg) {

  std::cout << "Hello world" << std::endl;
}

void thetrain_captcha(char *cfgfile, char *weightfile)
{
    float avg_loss = -1;
    srand(time(0));
    char *base = basecfg(cfgfile);
    printf("%s\n", base);
    network net = parse_network_cfg(cfgfile);
    if(weightfile){
        load_weights(&net, weightfile);
    }
    printf("Learning Rate: %g, Momentum: %g, Decay: %g\n", net.learning_rate, net.momentum, net.decay);
    int imgs = 1024;
    int i = *net.seen/imgs;
    list *plist = get_paths("/data/captcha/train.auto5");
    char **paths = (char **)list_to_array(plist);
    printf("%d\n", plist->size);
    time_t timenow;
    while(1){
        ++i;
        time(&timenow);
        data train = load_data_captcha(paths, imgs, plist->size, 10, 200, 60);
        translate_data_rows(train, -128);
        scale_data_rows(train, 1./128);
        printf("Loaded: %lf seconds\n", difftime(time(NULL)-timenow));
        time(&timenow);
        float loss = train_network(net, train);
        net.seen += imgs;
        if(avg_loss == -1) avg_loss = loss;
        avg_loss = avg_loss*.9 + loss*.1;
        printf("%d: %f, %f avg, %lf seconds, %d images\n", i, loss, avg_loss, difftime(time(NULL), timenow), net.seen);
        free_data(train);
        if(i%10==0){
            char buff[256];
            sprintf(buff, "/home/pjreddie/imagenet_backup/%s_%d.weights",base, i);
            save_weights(net, buff);
        }
    }
}