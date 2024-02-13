#ifndef SPOTFINDER_H
#define SPOTFINDER_H

#include <opencv2/core.hpp>

#include <random>

class SpotFinder {
public:
    /**
     * @brief pixels with a value less than this will be ignored in all methods.
     */
    double dark_threshold = 50;

    /**
     * @brief Number of pixels evaluated in the last finder-method called.
     */
    size_t num_px_evaluated = 0;

    /**
     * @brief Region of interested evaluated in the last call.
     */
    cv::Rect roi;

    SpotFinder();

    cv::Vec2d simpleHistogramFinder(cv::Mat_<uint8_t> const& img);

    cv::Vec2d defaultFinder(cv::Mat_<uint8_t> const& img);

    static void make_test_img(
            cv::Mat_<uint8_t>& img,
            cv::Vec2d const& center,
            std::mt19937_64& rng);
};

#endif // SPOTFINDER_H
