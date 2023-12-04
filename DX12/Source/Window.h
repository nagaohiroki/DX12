#pragma once
#include <Windows.h>
#include "tstring.h"
class Window {
public:
  Window();
  virtual ~Window();
  void Execute();
private:
  HWND m_hWnd = nullptr;
  bool Create(int w, int h, const std::tstring& name);
  bool ProccessWindow();
};
