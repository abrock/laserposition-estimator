#ifndef SPOTFINDER_H
#define SPOTFINDER_H

#include <opencv2/core.hpp>

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
};

#endif // SPOTFINDER_H
