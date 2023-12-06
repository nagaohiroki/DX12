#pragma once
#include <stdio.h>
#include <string>
#include <tchar.h>
#ifdef _UNICODE
#define tstring std::wstring
#define to_tstring std::to_wstring
#else
#define tstring std::string
#define to_tstring std::to_string
#endif
tstring to_tstr(const std::string &str);
tstring to_tstr(const std::wstring &str);
template <typename... Args> tstring tformat(const tstring &str, Args... args) {
  size_t size = 1024;
  tstring buf(size, 0);
  _sntprintf_s(&buf[0], size, size, str.c_str(), args...);
  return buf;
}
