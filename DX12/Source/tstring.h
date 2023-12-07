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
  tstring result(size, 0);
  size_t ret = _sntprintf_s(&result[0], size, size, str.c_str(), args...);
  return result;
}
