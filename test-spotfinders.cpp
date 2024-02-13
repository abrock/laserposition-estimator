#include <gtest/gtest.h>

#include "spotfinder.h"

#include <runningstats/runningstats.h>
namespace rs = runningstats;

#include <opencv2/highgui.hpp>

#include <random>

#include "misc.h"
using namespace Misc;

#include "cameramanager.h"

TEST(SpotFinder, simpleHistogramFinder) {
    std::mt19937_64 rng(0xBEEBBEEB);

    cv::Size const image_size(200,300);
    std::uniform_real_distribution<double> true_x_dist(image_size.width/3, (2*image_size.width)/3);
    std::uniform_real_distribution<double> true_y_dist(image_size.height/3, (2*image_size.height)/3);

    cv::Mat_<uint8_t> test_img(image_size);

    SpotFinder finder;

    rs::Stats2D<float> residuals_simple;

    for (size_t ii = 0; ii < 5'000; ++ii) {
        cv::Vec2d const solution(
                    true_x_dist(rng),
                    true_y_dist(rng)
                    );
        SpotFinder::make_test_img(test_img, solution, rng);
        cv::Vec2d const simple_solution = finder.simpleHistogramFinder(test_img);

        if (0 == ii) {
            cv::imwrite("test-spot-image.png", test_img);
            cv::imwrite("test-spot-image-roi.png", test_img(finder.roi));
            cv::Mat3b colored = CameraManager::color_exposure(test_img);
            cv::imwrite("test-spot-image-colored.png", colored);
            cv::imwrite("test-spot-image-colored-roi.png", colored(finder.roi));
        }


        cv::Vec2d const simple_diff = simple_solution - solution;
        residuals_simple.push_unsafe(simple_diff[0], simple_diff[1]);
    }

    rs::QuantileStats<float> res_x = residuals_simple.get1();
    rs::QuantileStats<float> res_y = residuals_simple.get1();

    res_x.plotHistAndCDF("simple-histogram-finder-x-residuals",
                         rs::HistConfig()
                         .setDataLabel("Residual [px]"));

    println("Residuals x: {}", res_x.print());
    println("Residuals y: {}", res_y.print());

    residuals_simple.plotHist("simple-histogram-finder-residuals",
                              {-1,-1},
                              rs::HistConfig()
                              .setTitle("Solution residuals")
                              .setXLabel("x [px]")
                              .setYLabel("y [px]")
                              .setFixedRatio());
}
