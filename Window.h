#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <functional>

#include "Render.h"

class Window {
public:
  Window(HINSTANCE hInstance, int nCmdShow, int width, int height, LPCWSTR title);
  HWND getHandle();
  void setRender(Render* render);
  void run();

private:
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  HWND hWnd;
  Render* render;
};