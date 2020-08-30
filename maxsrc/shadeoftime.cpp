//
// Created by max on 10.06.2020.
//
#include "shadeoftime.h"
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

int main(int narg, char *sarg[]){

    // Create a VideoCapture object and open the input file
    // If the input is the web camera, pass 0 instead of the video file name

    if(narg != 3){
        cerr << "Error Invalid syntax\n" << sarg[0] << " In Out" << endl;
        return -1;
    }

    VideoCapture cap(sarg[1]);

    // Check if camera opened successfully
    if(!cap.isOpened()){
        cerr << "Error opening input video stream or file" << endl;
        return -1;
    }

    double count=cap.get(cv::CAP_PROP_FRAME_COUNT);
    long frameHeight=cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    long frameWidth=cap.get(cv::CAP_PROP_FRAME_WIDTH);

    //cap.set(CAP_PROP_FPS, 200);

    //
    cv::VideoWriter makeVideo;
    makeVideo.open(sarg[2],
                   VideoWriter::fourcc('X','2','6','4'), 30, cv::Size(frameWidth, frameHeight), true);

//    for(int i = 0; i < 200; i++)
    //

    int n=0;
    Mat backlogImages;

    time_t tsBegin=clock();

    while(1){

        Mat frame;
        // Capture frame-by-frame
        cap >> frame;


        // If the frame is empty, break immediately
        if (frame.empty())
            break;
        Mat grayImage;

        if(n++==0) {
            backlogImages=Mat::zeros(frame.rows, frame.cols, frame.type());
            cvtColor(frame, grayImage, cv::COLOR_BGR2GRAY);
            // Display the resulting frame
            for(int x=0; x<frame.cols; x++) {
                for (int y = 0; y < frame.rows; y++) {
                    uchar color=grayImage.at<uchar>(y,x);
                    Vec3b &c=backlogImages.at<Vec3b>(y,x);
                    c[2]=color;
                    c[1]=color;
                    c[0]=color;
                    }
                }
            }
        else   {
            cvtColor(frame, grayImage, cv::COLOR_BGR2GRAY);
            // Display the resulting frame

            for(int x=0; x<frame.cols; x++) {
                for (int y = 0; y < frame.rows; y++) {
                    uchar color=grayImage.at<uchar>(y,x);
                    Vec3b &c=backlogImages.at<Vec3b>(y,x);
                    c[2]=c[1];
                    c[1]=c[0];
                    c[0]=color;
                }
            }
        }


       // imshow( "Frame", backlogImages );
      //  imshow( "FrameBW", grayImage );
       // makeVideo << backlogImages;
        if(n%100==0) {
            time_t tsNow=clock();
            long fps=n/((double)(tsNow-tsBegin)/CLOCKS_PER_SEC);
            cout << "FPS:" << fps;
            cout << "   Percent:" << n*100/count << "  ETA (minutes):" << (count-n)/fps/60 << endl;
        }

        // Press  ESC on keyboard to exit
        //char c=(char)waitKey(0);
        //if(c==27)
         //   break;
    }

    // When everything done, release the video capture object
    cap.release();
    makeVideo.release();

    // Closes all the frames
    //destroyAllWindows();

    return 0;
}