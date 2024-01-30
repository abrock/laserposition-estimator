#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <arv.h>

#include <stdlib.h>
#include <stdio.h>

#include <opencv2/highgui.hpp>

#include <QObject>
#include <QLabel>
#include <QMouseEvent>


class CameraManager : public QObject{
    Q_OBJECT

    ArvCamera *camera;
    GError *error = nullptr;

    bool stopped = false;

    std::string window_name = "img";

    double um_per_px = 2.4;

    bool camera_running = false;

    double requested_exposure = 10'000;

    double ref_a = 0;
    double ref_b = 100;
    double test_val = 50;

    cv::Vec2d current_pos;

    cv::Vec2d ref_a_pos{std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()};

    cv::Vec2d ref_b_pos{std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()};

public:
    void runCamera();

    void process_image(cv::Mat const& img);

    static cv::Mat3b color_exposure(cv::Mat1b const& input);

    void stop();
    void analyze(const cv::Mat1b &img, cv::Mat3b &colored);

    void runWaitKey();

    void increaseExposureTime();
    void decreaseExposureTime();

    void makeWindow();

    std::string makePosString(cv::Vec2d const& pos);

    void handlePositionResult(const cv::Vec2d &pos);

    Q_INVOKABLE void setExposure(const double exposure_us);
    Q_INVOKABLE void setRefA(const double val);
    Q_INVOKABLE void setRefB(const double val);
    Q_INVOKABLE void setTestVal(const double val);

    Q_INVOKABLE void assignRefA();
    Q_INVOKABLE void assignRefB();

signals:
    void positionDetected(QString const str);
    void refPosSetA(QString const str);
    void refPosSetB(QString const str);

    void testValEvaluated(QString const expected, QString const error);
};

#endif // CAMERAMANAGER_H
