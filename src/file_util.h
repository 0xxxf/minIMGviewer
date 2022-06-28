#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <vector>
#include <string>
#include <filesystem>

namespace miv {
  std::string get_wd(std::string path);

  bool is_file(std::string path);

  // Always check if parameter is a directory before calling this
  std::vector<std::string> load_all(std::string dir);
}

#endif
