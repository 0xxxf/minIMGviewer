#include "logger.h"

void miv::log_stdout(std::string msg, LOG_TYPE type) {
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
            << log_type_str << " | " << msg << '\n';
}
