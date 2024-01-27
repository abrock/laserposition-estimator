#ifndef MISC_H
#define MISC_H

#include <iostream>
#include <fmt/core.h>

#include <opencv2/core.hpp>

namespace Misc {
template <typename... Args>
inline void println(fmt::format_string<Args...> s, Args&&... args) {
    std::cout << fmt::format(s, std::forward<Args>(args)...) << std::endl;
}

template<class T, int N>
bool is_finite(cv::Vec<T, N> const& val) {
    for (size_t ii = 0; ii < N; ++ii) {
        if (!std::isfinite(val[ii])) {
            return false;
        }
    }
    return true;
}

}

#endif // MISC_H
