//
// Created by max on 10.06.2020.
//
#undef DEBUG
#include "shadeoftime.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <thread>
#include "tchannel.hpp"

using namespace std;
using namespace cv;

#pragma GCC optimization ("O3")
#pragma GCC optimization ("unroll-loops")

typedef tchannel::channel_limited<Mat,5> chanStage;
typedef tchannel::channel_limited<int,5> chanData;



void DoWrite(chanStage *in, char *path, int frameWidth, int frameHeight) {
    Mat image;
    cv::VideoWriter makeVideo;
    auto fcc=VideoWriter::fourcc('m','p','4','v');
//    auto fcc=VideoWriter::fourcc('h','2','6','4');
    makeVideo.open(path,
                   fcc, 30, cv::Size(frameWidth, frameHeight), true);

    while (in->read(image)) {
        makeVideo << image;
    }
    std::cout << "Closing writer" << std::endl;
    makeVideo.release();
}

void DoTest(chanData *out, int frameOffset, int frameGap) {
    int n=0;
    int w;
    do {
        n++;
        w=(n<frameOffset)?0:(((n-frameOffset)/frameGap)%2);
    //    std::cout << "-- "  << w << std::endl;

    }
    while (out->send(w));

    std::cout << "!! "  << n << std::endl;
}

void DoProcess(chanStage *in, chanStage *out, chanData *left, chanData *right, int frameWidth, int frameHeight) {
    Mat image;
//    long n=0;

    while (in->read(image)) {


        Mat grayImage;
        cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

        int fLeft, fRight;
        left->read(fLeft);
        right->read(fRight);


            // cvtColor(frame, grayImage, cv::COLOR_BGR2GRAY);
            // Display the resulting frame
            for (int i = 0; i < frameHeight; ++i)    {
                cv::Vec3b*  pixel  = image.ptr<cv::Vec3b>(i); // point to first pixel in row
                uchar*      pixelG = grayImage.ptr<uchar>(i);         // point to first color in row
//
                for (int j = 0; j < frameWidth; ++j)       {
                    uchar c = *pixelG++;
                    auto &pp=pixel[j];
                    bool g=false;

                    if(j<=frameWidth/2) {
                        g=(fLeft!=0);
                    } else {
                        g=(fRight!=0);
                    }

                    if(g) {
                        pp[2]=c;
                        pp[1]=c;
                        pp[0]=c;
                        }

                    }
                }


        out->send( image.clone() );
        }
    std::cout << "Closing output" << std::endl;
    out->close();
}


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

    chanStage stageRead, stageWrite;
    chanData  stageLeft, stageRight;

//    for(int i = 0; i < 200; i++)
    //

    int n=0;
    Mat backlogImages;

    std::thread ThProcessTest1(DoTest, &stageLeft, 1000, 300);
    std::thread ThProcessTest2(DoTest, &stageRight, 500, 300);

    std::thread ThWriter(DoWrite, &stageWrite, sarg[2], frameWidth, frameHeight);

    std::thread ThProcess(DoProcess, &stageRead, &stageWrite,  &stageLeft, &stageRight,  frameWidth, frameHeight);
    time_t tsBegin=clock();

    while(1)  {
        Mat frame;
        // Capture frame-by-frame
        cap >> frame;
        if (frame.empty())
            break;
        n++;
        stageRead.send(frame);

        // If the frame is empty, break immediately



        // imshow( "Frame", backlogImages );
        //  imshow( "FrameBW", grayImage );
        //stage.send(backlogImages);
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
    stageRead.close();
    ThProcess.join();
    ThWriter.join();
    // Closes all the frames
    //destroyAllWindows();
    return 0;

}