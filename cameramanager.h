#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <arv.h>

#include <stdlib.h>
#include <stdio.h>

#include <opencv2/highgui.hpp>

class CameraManager {
    ArvCamera *camera;
    GError *error = nullptr;

    bool stopped = false;

    std::string window_name = "img";

    double um_per_px = 2.4;

public:
    void runCamera();

    void process_image(cv::Mat const& img);

    cv::Mat3b color_exposure(cv::Mat1b const& input);

    void stop();
    void analyze(const cv::Mat1b &img, cv::Mat3b &colored);

    void runWaitKey();

    void increaseExposureTime();
    void decreaseExposureTime();

    void makeWindow();
};

#endif // CAMERAMANAGER_H
