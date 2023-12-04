#pragma once
#include "tstring.h"
#include <Windows.h>
class Window {
public:
  Window();
  virtual ~Window();
  void Execute();

private:
  HWND m_hWnd = nullptr;
  bool Create(int w, int h, const std::tstring &name);
  bool ProccessWindow();
  static LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
