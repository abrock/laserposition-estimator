#include "cameramanager.h"

#include <glog/logging.h>

#include <opencv2/imgproc.hpp>

#include "misc.h"
using namespace Misc;

#include <runningstats/runningstats.h>
namespace rs = runningstats;

void CameraManager::runCamera() {
    println("Running CameraManager::runCamera");

    camera = arv_camera_new (NULL, &error);

    CHECK(ARV_IS_CAMERA(camera));

    ArvStream *stream = NULL;

    println ("Found camera '{}'",
             arv_camera_get_model_name (camera, NULL));

    arv_camera_set_acquisition_mode (camera, ARV_ACQUISITION_MODE_CONTINUOUS, &error);
    println("arv_camera_set_acquisition_mode done.");

    /* Create the stream object without callback */
    stream = arv_camera_create_stream (camera, NULL, NULL, &error);
    CHECK_EQ(nullptr, error) << error->message;

    CHECK(ARV_IS_STREAM (stream));

    size_t payload;

    /* Retrieve the payload size for buffer creation */
    payload = arv_camera_get_payload (camera, &error);
    CHECK_EQ(nullptr, error) << error->message;

    /* Insert some buffers in the stream buffer pool */
    for (int i = 0; i < 5; i++)
        arv_stream_push_buffer (stream, arv_buffer_new (payload, NULL));

    arv_camera_start_acquisition (camera, &error);
    CHECK_EQ(nullptr, error) << error->message;

    for (size_t ii = 0; !stopped; ++ii) {
        ArvBuffer *buffer;

        buffer = arv_stream_pop_buffer(stream);
        if (ARV_IS_BUFFER (buffer)) {
            /* Display some informations about the retrieved buffer */
            int const width = arv_buffer_get_image_width (buffer);
            int const height = arv_buffer_get_image_height (buffer);
            println ("Acquired {}×{} buffer #{}",
                     width,
                     height,
                     ii);
            /* Don't destroy the buffer, but put it back into the buffer pool */
            arv_stream_push_buffer (stream, buffer);
            size_t size = 0;
            const void *raw = arv_buffer_get_data(buffer, &size);
            cv::Mat img_local;

            img_local.create(height, width, CV_8UC1);
            memcpy(img_local.ptr(0), raw, width*height);
            process_image(img_local);
        }
    }
    cv::destroyAllWindows();

    CHECK_EQ(nullptr, error) << error->message;
    arv_camera_stop_acquisition (camera, &error);
    CHECK_EQ(nullptr, error) << error->message;

    /* Destroy the stream object */
    g_clear_object (&stream);

    /* Destroy the camera instance */
    g_clear_object (&camera);
}

void CameraManager::process_image(const cv::Mat &img) {
    cv::Mat3b colored = color_exposure(img);
    analyze(img, colored);
    cv::imshow(window_name, colored);
    cv::waitKey(1);
}

cv::Mat3b CameraManager::color_exposure(const cv::Mat1b &input) {
    cv::Mat3b result;
    cv::merge(std::vector<cv::Mat1b>{input, input, input}, result);
    CHECK_EQ(input.size(), result.size());
    for (int ii = 0; ii < input.rows; ++ii) {
        for (int jj = 0; jj < input.cols; ++jj) {
            if (input(ii, jj) > 253) {
                result(ii, jj) = cv::Vec3b(0,0,255);
            }
            else {
                uint8_t const gamma_corrected = cv::saturate_cast<uint8_t>(std::round(std::sqrt(input(ii, jj))*std::sqrt(255)));
                result(ii, jj) = cv::Vec3b(gamma_corrected, gamma_corrected, gamma_corrected);
            }
        }
    }
    return result;
}

void CameraManager::analyze(cv::Mat1b const& img, cv::Mat3b& colored) {
    rs::MeanFromHist x_stats;
    rs::MeanFromHist y_stats;
    double const px2mm_factor = um_per_px / 1'000;
    for (int ii = 0; ii < img.rows; ++ii) {
        double const y_mm = double(ii) * px2mm_factor;
        for (int jj = 0; jj < img.cols; ++jj) {
            if (img(ii, jj) > 50) {
                double const x_mm = double(jj) * px2mm_factor;
                x_stats.push_unsafe(x_mm, img(ii, jj));
                y_stats.push_unsafe(y_mm, img(ii, jj));
            }
        }
    }
    double const px_pos_x = x_stats.getMean() / px2mm_factor;
    double const px_pos_y = y_stats.getMean() / px2mm_factor;
    println("Evaluated {}px ({}%)", x_stats.getCount(), 100.0 * double(x_stats.getCount()) / (img.rows * img.cols));
    println("X pos: {} ({}px)", x_stats.getMean(), px_pos_x);
    println("Y pos: {} ({}px)", y_stats.getMean(), px_pos_y);
    cv::Scalar const line_color(255, 0, 0);
    cv::line(colored, cv::Point(px_pos_x, 1), cv::Point(px_pos_x, img.size().height-2), line_color, 13);
    cv::line(colored, cv::Point(1, px_pos_y), cv::Point(img.size().width-2, px_pos_y), line_color, 13);
}

void CameraManager::runWaitKey() {
    while (!stopped) {
        char const key = cv::waitKey(10'000);
        switch (key) {
        case '+':
            increaseExposureTime();
            break;
        case '-':
            decreaseExposureTime();
            break;
        case 'q':
            stopped = true;
            cv::destroyAllWindows();
            break;
        }
        println("Key: {} ({})", key, int(key));
    }
}

void CameraManager::increaseExposureTime() {
    println("Increasing exposure time...");
    GError* error = nullptr;
    double const exp = arv_camera_get_exposure_time(camera, &error);
    CHECK_EQ(nullptr, error) << error->message;
    println("Current exp: {}", exp);
    double const new_exp = exp*1.2;
    arv_camera_set_exposure_time(camera, new_exp, &error);
    CHECK_EQ(nullptr, error) << error->message;
    println("Increased exposure from {} to {}", exp, new_exp);
}

void CameraManager::decreaseExposureTime() {
    println("Decreasing exposure time...");
    GError* error = nullptr;
    double const exp = arv_camera_get_exposure_time(camera, &error);
    CHECK_EQ(nullptr, error) << error->message;
    println("Current exp: {}", exp);
    double const new_exp = exp/1.2;
    arv_camera_set_exposure_time(camera, new_exp, &error);
    CHECK_EQ(nullptr, error) << error->message;
    println("Decreased exposure from {} to {}", exp, new_exp);
}

void CameraManager::makeWindow() {
    cv::namedWindow(window_name);
}

void CameraManager::stop() {
    stopped = true;
}
