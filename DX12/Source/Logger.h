#pragma once
#include "tstring.h"
class Logger {
public:
  Logger() {}
  ~Logger() {}
  static void Log(const std::tstring &log);
  static void Log(const TCHAR* log);
};
