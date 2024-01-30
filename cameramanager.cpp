#include "cameramanager.h"

#include <glog/logging.h>

#include <opencv2/imgproc.hpp>

#include "misc.h"
using namespace Misc;

#include <runningstats/runningstats.h>
namespace rs = runningstats;

#include "spotfinder.h"

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

    camera_running = true;
    setExposure(requested_exposure);

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
    SpotFinder finder;
    double const px2mm_factor = 1'000.0 / um_per_px;
    current_pos = finder.defaultFinder(img) * px2mm_factor;
    double const px_pos_x = current_pos[0] / px2mm_factor;
    double const px_pos_y = current_pos[1] / px2mm_factor;
    println("Evaluated {}px ({}%)",
            finder.num_px_evaluated,
            100.0 * double(finder.num_px_evaluated) / (img.rows * img.cols));
    println("X pos: {} ({}px)", current_pos[0], px_pos_x);
    println("Y pos: {} ({}px)", current_pos[0], px_pos_y);
    cv::Scalar const line_color(255, 0, 0);
    cv::line(colored, cv::Point(px_pos_x, 1), cv::Point(px_pos_x, img.size().height-2), line_color, 1);
    cv::line(colored, cv::Point(1, px_pos_y), cv::Point(img.size().width-2, px_pos_y), line_color, 1);

    handlePositionResult(current_pos);
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

std::string CameraManager::makePosString(const cv::Vec2d &pos) {
    return fmt::format("x={:.3f}mm, y={:.3f}mm", pos[0], pos[1]);
}

void CameraManager::handlePositionResult(cv::Vec2d const& pos) {
    emit positionDetected(QString::fromStdString(makePosString(pos)));

    if (!is_finite(ref_a_pos) || !is_finite(ref_b_pos)) {
        return;
    }

    cv::Vec2d const expected = (ref_a_pos * (ref_b - test_val) + ref_b_pos * (test_val - ref_a)) / (ref_b - ref_a);
    cv::Vec2d const error = pos - expected;

    emit testValEvaluated(
                QString::fromStdString(makePosString(expected)),
                QString::fromStdString(makePosString(error))
                              );
}

void CameraManager::setExposure(double const exposure_us) {
    requested_exposure = exposure_us;
    if (!camera_running) {
        println("camera not running");
        return;
    }
    if (!ARV_IS_CAMERA(camera)) {
        println("camera not valid");
        return;
    }
    GError* error = nullptr;
    arv_camera_set_exposure_time(camera, exposure_us, &error);
    CHECK_EQ(nullptr, error) << error->message;
    println("Set exposure time to {}", exposure_us);
}

void CameraManager::setRefA(const double val) {
    ref_a = val;
}

void CameraManager::setRefB(const double val) {
    ref_b = val;
}

void CameraManager::setTestVal(const double val) {
    test_val = val;
}

void CameraManager::assignRefA() {
    ref_a_pos = current_pos;
    emit refPosSetA(QString::fromStdString(makePosString(ref_a_pos)));
}

void CameraManager::assignRefB() {
    ref_b_pos = current_pos;
    emit refPosSetB(QString::fromStdString(makePosString(ref_b_pos)));
}

void CameraManager::stop() {
    stopped = true;
}
