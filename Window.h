#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <functional>

#include "Render.h"

class Window {
public:
  BOOL init(HINSTANCE hInstance, LPCWSTR title);
  void show(int nCmdShow, int width, int height);

  HWND getHandle() const;
  void setRender(Render* render);
  
  BOOL run();

private:
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  HWND m_hWnd{ nullptr };
  Render* m_render{ nullptr };
};