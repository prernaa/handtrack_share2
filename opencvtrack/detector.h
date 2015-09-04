#pragma once
#include <QDebug>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/gpu/gpu.hpp"


class Detector
{
    std::string trained_cascade_name;
    //cv::gpu::CascadeClassifier_GPU cascade;
    cv::CascadeClassifier cascade;
public:
    Detector();
    std::vector<cv::Rect> detectInFrame(cv::Mat frame);
};
