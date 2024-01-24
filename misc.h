#ifndef MISC_H
#define MISC_H

#include <iostream>
#include <fmt/core.h>

namespace Misc {
template <typename... Args>
inline void println(fmt::format_string<Args...> s, Args&&... args) {
    std::cout << fmt::format(s, std::forward<Args>(args)...) << std::endl;
}

}

#endif // MISC_H
