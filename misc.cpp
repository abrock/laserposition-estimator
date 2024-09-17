#include "misc.h"

namespace Misc {

std::vector<std::string> splitString(const std::string &in, const char s) {
  std::string current;
  std::vector<std::string> result;
  for (size_t ii = 0; ii < in.size(); ++ii) {
    if (in[ii] == s) {
      if (current.size() > 0) {
        result.push_back(current);
      }
      current.clear();
    }
    else {
      current += in[ii];
    }
  }
  if (current.size() > 0) {
    result.push_back(current);
  }
  return result;
}

}
