#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <chrono>
#include <iomanip>

namespace miv {
  enum LOG_TYPE { 
    FS, 
    EVENT,
    RESOURCE
  };

  void log_stdout(std::string msg, LOG_TYPE type);
  void log_stdout(std::string title, std::string value, LOG_TYPE type);

  void log_file(std::string msg, std::string path, LOG_TYPE type);
}

#endif