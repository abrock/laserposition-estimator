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

void SpotFinder::make_test_img(cv::Mat_<uint8_t> &img, const cv::Vec2d &center, std::mt19937_64 &rng) {
    double const min = 25;
    double const max = 260;
    double const noise_factor = 1;
    double const sigma = 10;
    for (int yy = 0; yy < img.rows; ++yy) {
        for (int xx = 0; xx < img.cols; ++xx) {
            cv::Vec2d const pos(xx,yy);
            cv::Vec2d const diff = pos - center;
            double const value = min + (max-min)*std::exp(-diff.ddot(diff)/(2*sigma*sigma));
            img(yy,xx) = cv::saturate_cast<uint8_t>(
                        std::normal_distribution<double>(value, noise_factor*sqrt(value))(rng)
                        );
        }
    }
}
