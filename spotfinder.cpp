#include "spotfinder.h"

#include <runningstats/runningstats.h>
namespace rs = runningstats;

#include <ceres/ceres.h>

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

struct GaussCost {
    int _pos_x;
    int _pos_y;
    float value;

    template<class T>
    static T square(T const val) {
        return val*val;
    }

    template<class T>
    bool operator()(
            const T* const pos,
            const T* const sigma,
            const T* const scale,
            const T* const offset,
            T* residuals) const {
        T const diff_x = square((T(_pos_x) - pos[0])/sigma[0]);
        T const diff_y = square((T(_pos_y) - pos[1])/sigma[1]);
        T const func_val = ceres::exp(-(diff_x + diff_y)/T(2));

        residuals[0] =
                T(-value)
                + offset[0]
                + scale[0] * func_val;

        return true;
    }

    static ceres::CostFunction* create(int const pos_x, int const pos_y, float value) {
      //     std::cout << wp << ip << "\n";
      return (new ceres::AutoDiffCostFunction<GaussCost, 1, 2, 2, 1, 1>(
        new GaussCost(pos_x, pos_y, value)));
    }

};

cv::Vec2d SpotFinder::ceresFitFinder(const cv::Mat_<uint8_t> &img) {
    ceres::Problem problem;

    cv::Vec2d result = simpleHistogramFinder(img);

    cv::Vec2d sigma{roi.width/2, roi.height/2};

    double scale = 255;
    double offset = 0;

    for (int yy = roi.tl().y; yy < roi.br().y; ++yy) {
        for (int xx = roi.tl().x; xx < roi.br().x; ++xx) {
            problem.AddResidualBlock(
                        GaussCost::create(xx, yy, img(yy, xx)),
                        nullptr,
                        result.val,
                        sigma.val,
                        &scale,
                        &offset
                        );
        }
    }
    ceres::Solver::Options ceres_opts;

    ceres::Solver::Summary summary;

    ceres_opts.minimizer_progress_to_stdout = false;
    ceres_opts.linear_solver_type = ceres::DENSE_SCHUR;

    ceres_opts.max_num_iterations = 1000;
    ceres_opts.max_num_consecutive_invalid_steps = 100;
    double const relative_tolerance = 1e-1;
    ceres_opts.function_tolerance *= relative_tolerance;
    ceres_opts.gradient_tolerance *= relative_tolerance;
    ceres_opts.parameter_tolerance *= relative_tolerance;

    problem.SetParameterBlockConstant(result.val);
    problem.SetParameterBlockConstant(sigma.val);
    ceres::Solve(ceres_opts, &problem, &summary);

    problem.SetParameterBlockVariable(sigma.val);
    ceres::Solve(ceres_opts, &problem, &summary);

    problem.SetParameterBlockVariable(result.val);
    ceres::Solve(ceres_opts, &problem, &summary);

    //std::cout << summary.FullReport() << std::endl;

    return result;
}
