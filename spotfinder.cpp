#include "spotfinder.h"

#include <runningstats/runningstats.h>
namespace rs = runningstats;

SpotFinder::SpotFinder()
{

}

cv::Vec2d SpotFinder::simpleHistogramFinder(const cv::Mat_<uint8_t> &img) {
    rs::MeanFromHist x_stats;
    rs::MeanFromHist y_stats;
    for (int yy = 0; yy < img.rows; ++yy) {
        for (int xx = 0; xx < img.cols; ++xx) {
            if (img(yy, xx) > 50) {
                x_stats.push_unsafe(xx, img(yy, xx));
                y_stats.push_unsafe(yy, img(yy, xx));
            }
        }
    }
    num_px_evaluated = x_stats.getBinCount();
    roi = cv::Rect(
                cv::Point(x_stats.getMin(), y_stats.getMin()),
                cv::Point(x_stats.getMax(), y_stats.getMax())
                );
    return {x_stats.getMean(), y_stats.getMean()};
}

cv::Vec2d SpotFinder::defaultFinder(const cv::Mat_<uint8_t> &img) {
    return simpleHistogramFinder(img);
}
