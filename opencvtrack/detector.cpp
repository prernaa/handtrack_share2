#include "detector.h"
using namespace cv;
Detector::Detector()
{
    /// filename of the trained classifer
    trained_cascade_name = "/Users/prerna/Desktop/Haar_faces/OpenPalmsDataset/cascade_both1/cascade.xml";
    //trained_cascade_name = "/Users/prerna/Desktop/haar_old stuff/haarxml_try/haarcascade_frontalface_alt2.xml";

    /// loading the cascade
    if( !cascade.load( trained_cascade_name ) ){ qDebug()<<"Error loading cascade file!"; return; };

    qDebug()<<"Cascade loaded";
}

std::vector<Rect> Detector::detectInFrame(Mat frame){
    std::vector<Rect> hands;
    Mat frame_gray; // Haar works on grayscale images
    cvtColor(frame, frame_gray, CV_RGB2GRAY);
    equalizeHist(frame_gray, frame_gray);
    cascade.detectMultiScale( frame_gray, hands, 1.1, 3, 0|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(40,40),cv::Size(100,100));
    return hands;
    /*for( int i = 0; i < hands.size(); i++ )
    {
        cv::rectangle(frame, hands[i], Scalar(255,0,255), 1, 8, 0);
    }*/
}
