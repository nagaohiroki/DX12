#include "Logger.h"
#include <Windows.h>
void Logger::Log(const tstring &log) {
  OutputDebugString(log.c_str());
  Line();
  record += log + _T("\n");
}
void Logger::Log(tstring &log) { Log(log.c_str()); }
void Logger::Line() { OutputDebugString(_T("\n")); }
