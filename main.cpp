#include "Window.h"
#include "Render.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
  Window window(hInstance, nCmdShow, 1280, 720, L"DX11 Window");
  Render render(window.getHandle());

  window.setRender(&render);
  window.run();

  return 0;
}
