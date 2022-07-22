#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

/* LOG FORMAT: 
   DATE TIME | LOG TYPE | MESSAGE
*/
namespace miv {
enum LOG_TYPE { FS , EVENT , RESOURCE, ERR };
void log_stdout(std::string msg, LOG_TYPE type);

template <typename T> void log_stdout(std::string title, T value, LOG_TYPE type) {
  auto time = std::chrono::system_clock::now();
  std::time_t now = std::chrono::system_clock::to_time_t(time);
  std::string log_type_str = "NO TYPE";

  if (type == FS) {
    log_type_str = "\033[1;36mFILE\033[0m"; 
  }
  if (type == EVENT) {
    log_type_str = "\033[1;32mEVENT\033[0m";
  }
  if (type == RESOURCE) {
    log_type_str = "\033[1;35mRESOURCE\033[0m";
  }
  if (type == ERR) {
    log_type_str = "\031[1;32mERROR\033[0m";
  }

  std::cout << std::put_time(std::localtime(&now), "%c") << " | "
            << log_type_str << " | " << title << " => " << value << '\n';
}

void log_file(std::string msg, std::string path, LOG_TYPE type);
} // namespace miv

#endif