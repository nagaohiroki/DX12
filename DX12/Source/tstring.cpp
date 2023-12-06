#include "tstring.h"
#include <stdlib.h>
tstring to_tstr(const std::string& str)
{
#ifdef _UNICODE
  size_t size = 0;
  const size_t strLen = str.length();
  const char *cStr = str.c_str();
  mbstowcs_s(&size, nullptr, 0, cStr, strLen);
  std::wstring wstr(size, 0);
  mbstowcs_s(&size, &wstr[0], size, cStr, strLen);
  return wstr;
#else
  return str;
#endif
}
tstring to_tstr(const std::wstring& wstr)
{
#ifdef _UNICODE
  return wstr;
#else
  size_t size = 0;
  const size_t wstrLen = wstr.length();
  const wchar_t *cWstr = wstr.c_str();
  wcstombs_s(&size, nullptr, 0, cWstr, wstrLen);
  std::string str(size, 0);
  wcstombs_s(&size, &str[0], size, cWstr, wstrLen);
  return str;
#endif
}
