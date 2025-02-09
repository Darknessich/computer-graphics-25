#include <cassert>

#include "Window.h"

Window::Window(HINSTANCE hInstance, int nCmdShow, int width, int height, LPCWSTR title) 
  : render(nullptr)
{
  WNDCLASS wc = {};
  wc.lpfnWndProc = Window::WndProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = L"DX11WindowClass";
  RegisterClass(&wc);

  hWnd = CreateWindowW(
    L"DX11WindowClass", title, WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
    nullptr, nullptr, hInstance, nullptr
  );
  
  assert(hWnd != nullptr);
  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  RECT rc;
  rc.left = 0;
  rc.top = 0;
  rc.right = width;
  rc.bottom = height;

  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, TRUE);

  MoveWindow(hWnd, 100, 100, rc.right - rc.left, rc.bottom - rc.top, TRUE);
}

HWND Window::getHandle() const{
  return hWnd;
}

void Window::setRender(Render* render) {
  this->render = render;
}

void Window::run() {
  assert(render != nullptr);
  MSG msg = {};
  while (msg.message != WM_QUIT) {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    
    render->render();
  }
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
  switch (message)
  {
  case WM_SIZE:
    if (window && window->render) {
      RECT rc;
      GetClientRect(hWnd, &rc);
      window->render->resize(rc.right - rc.left, rc.bottom - rc.top);
    }
    return 0;

  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  }

  return DefWindowProc(hWnd, message, wParam, lParam);
}

