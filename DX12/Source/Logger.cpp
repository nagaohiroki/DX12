#include "Logger.h"
#include <Windows.h>
void Logger::Log(const std::tstring &log) {
  OutputDebugString(log.c_str());
  OutputDebugString(_T("\n"));
}
void Logger::Log(const TCHAR *log) {
  OutputDebugString(log);
  OutputDebugString(_T("\n"));
}
