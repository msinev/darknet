//
// Created by max on 10.06.2020.
//
#include "shadeoftime.h"
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;
int delayKey=1;
long framebegin=0;
long framecount=0;

int main(int narg, char *sarg[]){

    // Create a VideoCapture object and open the input file
    // If the input is the web camera, pass 0 instead of the video file name

    if(narg != 2){
        cerr << "Error Invalid syntax\n" << sarg[0] << " In" << endl;
        return -1;
    }

    VideoCapture cap(sarg[1]);

    // Check if camera opened successfully
    if(!cap.isOpened()){
        cerr << "Error opening input video stream or file" << endl;
        return -1;
    }

    double count=cap.get(cv::CAP_PROP_FRAME_COUNT);
//    long frameHeight=cap.get(cv::CAP_PROP_FRAME_HEIGHT);
//    long frameWidth=cap.get(cv::CAP_PROP_FRAME_WIDTH);

    //cap.set(CAP_PROP_FPS, 200);
    int n=0;
    Mat backlogImages;

    time_t tsBegin;
    time(&tsBegin);

    while(1){

        Mat frame;
        // Capture frame-by-frame
        cap >> frame;


        // If the frame is empty, break immediately
        if (frame.empty())
            break;

        imshow( "Frame", frame );
        waitKey(delayKey);
      //  imshow( "FrameBW", grayImage );
        if(n++%100==0) {
            time_t tsNow;
            time(&tsNow);
            long fps=n/difftime(tsNow, tsBegin);
            cout << "FPS:" << fps;
            cout << "   Percent:" << n*100/count << "  ETA (minutes):" << (count-n)/fps/60 << endl;
        }
    }
    // When everything done, release the video capture object
    cap.release();
    // Closes all the frames
    destroyAllWindows();
    return 0;
}