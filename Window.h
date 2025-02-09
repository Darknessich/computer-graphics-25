#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <functional>

class Window {
public:
  Window(HINSTANCE hInstance, int nCmdShow, int width, int height, const char* title);
  HWND getHandle();
  void run(const std::function<void()>& renderCallback);

private:
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  HWND hWnd;
};