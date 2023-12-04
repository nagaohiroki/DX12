#pragma once
#include <string>
#include <tchar.h>
namespace std
{
#ifdef _UNICODE
    typedef wstring tstring;
#else
    typedef string tstring;
#endif
}
