#include "Window.h"
#include "GraphicDevice.h"
#include "Logger.h"
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                   _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  CoInitializeEx(nullptr, COINIT_MULTITHREADED);
  Window w;
  w.Execute();
  CoUninitialize();
  return 0;
}
Window::Window() {}

Window::~Window() {}

void Window::Execute() {
  const std::tstring name = _T("DX12");
  const UINT width = 1920;
  const UINT height = 1080;
  Create(width, height, name);
  GraphicDevice graphicDevice;
  if (!graphicDevice.Create(m_hWnd, width, height)) {
    return;
  }
  while (true) {
    if (!ProccessWindow()) {
      break;
    }
    graphicDevice.ScreenFlip();
  }
}
bool Window::Create(int w, int h, const std::tstring &name) {
  HINSTANCE hInstance = GetModuleHandle(nullptr);
  WNDCLASSEX wcex = {0};
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.lpfnWndProc = Window::WindowProc;
  wcex.lpszClassName = name.c_str();
  wcex.hInstance = hInstance;
  if (!RegisterClassEx(&wcex)) {
    Logger::Log(_T("failed to register class"));
    return false;
  }
  m_hWnd = CreateWindow(name.c_str(), name.c_str(), WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, 0, w, h, nullptr, nullptr, hInstance,
                        nullptr);
  if (m_hWnd == nullptr) {
    Logger::Log(_T("failed to create window"));
    return false;
  }
  ShowWindow(m_hWnd, SW_SHOW);
  UpdateWindow(m_hWnd);
  return true;
}
bool Window::ProccessWindow() {
  MSG msg = {0};
  while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
    switch (msg.message) {
    case WM_QUIT:
      return false;
    default:
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      break;
    }
  }
  return true;
}
LRESULT Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  default:
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
  return 0;
}
