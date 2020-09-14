//
// Created by max on 13.02.19.
//
#include <stdio.h>
#include <cv.h>
#include <iostream>
#include <highgui.h>

int breaker(int argc, char **argv) {
    /*
    if(argc!=2) {
        printf("PATH {video file}\n");
        return 1;
    }*/
    const int compressImgXY=128;
    IplImage *baseFrame      =NULL;
    IplImage *diffRGBFrame   =NULL;
    IplImage *diffGrayFrame  =NULL;
    IplImage *diffTGrayFrame =NULL;
    IplImage *compressFrame  =NULL;

    CvCapture* capture=0;
    IplImage* frame=0;
    capture = cvCaptureFromAVI("/home/max/tennis2.mp4"); // read AVI video
    if( !capture )
        throw "Error when reading steam_avi";

    cvNamedWindow( "w", 1);
    int t=1000/25;
    for(int n=0; true; n++)
    {
        frame = cvQueryFrame( capture );
        if(!frame) {
            printf("No more frame(s)\n");
            break;
        }
        cvShowImage("w", frame);
        char c=cvWaitKey(t); // key press ESC to close window

        if(c=='+' || c==-85) {
            t=(t==0)?1:int(float(t)*1.1+10);
        }
        else if(c=='-' || c==-83) {
            t=int(float(t)*0.8);
        }
        else if(c=='.' || c==-97) {
            t=1000/25;
        }
        else if(c==' ') {
            t=0;
        }
        else if(c==27) {
            printf("Exiting %d\n", (int)c);
            break;
          }
        else if(c!=-1) {
            printf("Key %d\n", (int)c);
        }

        bool flagSave=false;
        long nzCount=0;
        if(n==0) {
            printf("Creating Using Frame Channels %d\n", (int)frame->nChannels);

            baseFrame      = cvCreateImage ( cvSize(compressImgXY , compressImgXY ), frame->depth, frame->nChannels );
            diffRGBFrame      = cvCreateImage ( cvSize(compressImgXY , compressImgXY ), frame->depth, frame->nChannels);
            diffGrayFrame      = cvCreateImage ( cvSize(compressImgXY , compressImgXY ),  IPL_DEPTH_8U, 1);
            diffTGrayFrame     = cvCreateImage ( cvSize(compressImgXY , compressImgXY ),  IPL_DEPTH_8U, 1);
            compressFrame  = cvCreateImage ( cvSize(compressImgXY , compressImgXY ), frame->depth, frame->nChannels );
            cvResize(frame, baseFrame);

        } else {

            if(n%32==0) {
            cvResize(frame, compressFrame);
            cvAbsDiff(compressFrame,baseFrame,diffRGBFrame);
            cvCvtColor(diffRGBFrame, diffGrayFrame, CV_RGB2GRAY);
            cvThreshold(diffGrayFrame, diffTGrayFrame, 24, 255, CV_THRESH_BINARY);

            nzCount=cvCountNonZero(diffTGrayFrame);
            if (nzCount > 8000) {
                char path[128];
                sprintf(path, "/home/max/break/frame%05dz.png", n);
                cvSaveImage(path, diffTGrayFrame);
                flagSave=true;
                }
            IplImage *exFrame = baseFrame;
            baseFrame = compressFrame;
            compressFrame = exFrame;

            }

        }
        if(/*n%100==0 ||*/ flagSave) {
            IplImage *destination = cvCreateImage ( cvSize(compressImgXY , compressImgXY ), frame->depth, frame->nChannels );


            cvResize(frame, destination);
            char path[128];
            sprintf(path, "/home/max/break/frame%05d.png", n);
            cvSaveImage(path, destination);
            cvReleaseImage(&destination);
            sprintf(path, "/home/max/break/frame%05d.jpg", n);
            cvSaveImage(path, frame);
        }
        printf("Frame %d / %d/ %lu\n", n, t, nzCount);

    }

    if(baseFrame==NULL) {
        cvReleaseImage(&baseFrame);
        cvReleaseImage(&diffRGBFrame);
        cvReleaseImage(&diffGrayFrame);
        cvReleaseImage(&diffTGrayFrame);
    }

    cvDestroyWindow("w");
    cvReleaseImage(&frame);

    return 0;
}