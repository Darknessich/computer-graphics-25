#include <cassert>

#include "Window.h"

Render* Window::m_pRender = nullptr;

BOOL Window::init(HINSTANCE hInstance, LPCWSTR title) 
{
  const LPCWSTR CLASS_NAME = L"DX11WindowClass";
  
  WNDCLASS wc = {};
  wc.lpfnWndProc = Window::WndProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;
  
  if (!RegisterClass(&wc))
  {
    return FALSE;
  }

  m_hWnd = CreateWindowW(
    CLASS_NAME, title, WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
    nullptr, nullptr, hInstance, nullptr
  );

  return m_hWnd != nullptr;
}

void Window::show(int nCmdShow, int width, int height) 
{
  assert(m_hWnd != nullptr);

  const int X = 100, Y = 100;

  ShowWindow(m_hWnd, nCmdShow);
  UpdateWindow(m_hWnd);

  RECT rc;
  rc.left = 0;
  rc.top = 0;
  rc.right = width;
  rc.bottom = height;

  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, TRUE);
  MoveWindow(m_hWnd, X, Y, rc.right - rc.left, rc.bottom - rc.top, TRUE);
}

HWND Window::getHandle() const
{
  return m_hWnd;
}

void Window::setRender(Render* pRender)
{
  m_pRender = pRender;
}

BOOL Window::run() 
{
  assert(m_pRender != nullptr);
  
  MSG msg = {};
  while (msg.message != WM_QUIT) 
  {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) 
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    
    m_pRender->render();
  }

  return static_cast<BOOL>(msg.wParam);
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
  switch (message)
  {
  case WM_SIZE:
    if (m_pRender != nullptr) 
    {
      RECT rc;
      GetClientRect(hWnd, &rc);
      m_pRender->resize(rc.right - rc.left, rc.bottom - rc.top);
    }
    return 0;

  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  }

  return DefWindowProc(hWnd, message, wParam, lParam);
}
