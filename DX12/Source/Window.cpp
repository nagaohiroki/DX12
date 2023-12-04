#include "Window.h"
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
  Window w;
  w.Execute();
  return 0;
}
Window::Window() {}

Window::~Window() {}

LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  default:
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
  return 0;
}
bool Window::Create(int w, int h, const std::tstring &name) {
  HINSTANCE hInstance = GetModuleHandle(nullptr);
  WNDCLASSEX wcex = {0};
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.lpfnWndProc = WindowProc;
  wcex.lpszClassName = name.c_str();
  wcex.hInstance = hInstance;
  if (!RegisterClassEx(&wcex)) {
    return false;
  }
  m_hWnd = CreateWindow(name.c_str(), name.c_str(), WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, 0, w, h, nullptr, nullptr, hInstance,
                        nullptr);
  if (m_hWnd == nullptr) {
    return false;
  }
  ShowWindow(m_hWnd, SW_SHOW);
  UpdateWindow(m_hWnd);
  return true;
}
void Window::Execute() {
  const std::tstring name = _T("DX12");
  Create(1920, 1080, name);

  MSG msg = {0};
  while (msg.message != WM_QUIT) {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
}
