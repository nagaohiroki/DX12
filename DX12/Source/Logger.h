#pragma once
#include "tstring.h"
class Logger {
public:
  Logger() {}
  ~Logger() {}
  static void Log(const tstring &log);
  static void Log(tstring &log);
  template <typename... Args>
  static void Log(const tstring &str, Args... args) {
    Log(tformat(str, args...).c_str());
  }
private:
  static void Line();
};
