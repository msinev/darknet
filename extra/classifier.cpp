//
// Created by max on 23.05.2020.
//
#ifndef GPU
#error fatal no GPU in build
#endif
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
#include <boost/program_options.hpp>
#include <sys/time.h>
#include <time.h>
#include <fstream>
#include "opencv2/opencv.hpp"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

extern "C" image mat_to_image(cv::Mat mat);  // not declared in headers AFAIK
//extern "C" cv::Mat image_to_mat(image img);

int main(int ac, char **av) {

    namespace po = boost::program_options;
    std::string net_cfg_path, net_weight_path, video_path, log_path("classes.data");
    po::options_description desc("options");
//    std::string task_type;
    try {
        desc.add_options()
                ("help,h", "Show help")
                ("cfg,c", po::value<std::string>(&net_cfg_path), "Network config file")
                ("video", po::value<std::string>(&video_path), "Video file")
                ("out", po::value<std::string>(&log_path), "Data output file")
                ("weights,w", po::value<std::string>(&net_weight_path), "Network weights file");

        po::positional_options_description pos_desc;
        pos_desc.add("video", 1);
        pos_desc.add("out", 1);

        po::parsed_options parsed = po::command_line_parser(ac, av).options(desc).positional(pos_desc).run();
        po::variables_map vm;
        po::store(parsed, vm);
        po::notify(vm);

        if (net_cfg_path.empty() || net_weight_path.empty() || vm.count("video") != 1 || vm.count("help")) {
            std::cout << "Cmd [options] video output\n" << desc << std::endl;
            return 1;
        }

    } catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
        return 2;
    }

    network net = parse_network_cfg_custom((char *) net_cfg_path.c_str(),1, 1);
    load_weights(&net, (char *) net_weight_path.c_str());
    std::cout << "Opening " << video_path << std::endl;

    cv::VideoCapture cap(video_path);

    if (!cap.isOpened()) {
        std::cerr << "Error opening input video stream or file" << std::endl;
        return -1;
    }

    double count = cap.get(cv::CAP_PROP_FRAME_COUNT);
    long frameHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    long frameWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH);

    std::cout << "Frame size " << frameWidth << "x" << frameHeight << std::endl;
    std::cout << "Frames to go " << count << std::endl;
    uint64_t countDoneFrame = 0;

    std::ofstream classesOut(log_path);

    time_t begin, mark;
    time(&begin);
    mark=begin;

    const int topk=2;
    int indexes[topk];

    while (1) {


        cv::Mat frame;
        // Capture frame-by-frame
        cap >> frame;

        // If the frame is empty, break immediately
        if (frame.empty())    break;
        cv::Mat matResized;
        //resize(src, dst, Size(), 2, 2, INTER_CUBIC); // upscale 2x
        resize(frame, matResized, cv::Size(net.w, net.h), 0, 0, cv::INTER_CUBIC);
        image out = mat_to_image(matResized); // byte data to float data
                                         // TODO Might be we should resize first or in concurrent thread
        //int size = net.w;
        //image resized = resize_image(out, net.w, net.h);
        //if(!countFrame) resize_network(&net, resized.w, resized.h);
        //show_image(im, "orig");
        //show_image(crop, "cropped");
        //cvWaitKey(0);
        float *pred = network_predict(net, out.data);
        //top_k(pred, 7, 2, indexes);


        std::string prefix = "[ ";
        for(int i=0; i<7; i++) {
            classesOut << prefix << pred[i];
            prefix=", ";
            }
        classesOut  << "]" << std::endl;
        free_image(out);
//        free_image(resized);

        countDoneFrame++;
        time_t tnow;
        time(&tnow);

        if(difftime(tnow, mark)>2) {
            mark=tnow;
            auto fps= countDoneFrame / difftime(tnow, begin);
            std::cout << " At frame " << countDoneFrame << " FPS " << fps << " ETA " << (count - countDoneFrame)/fps/60 <<" minutes" << std::endl;
            }

        }
    }

