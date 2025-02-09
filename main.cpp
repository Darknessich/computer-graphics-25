#include "Window.h"
#include "Render.h"

int APIENTRY wWinMain(
  _In_ HINSTANCE hInstance, 
  _In_opt_ HINSTANCE hPrevInstance, 
  _In_ LPWSTR lpCmdLine, 
  _In_ int nCmdShow
) 
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  Window window(hInstance, nCmdShow, 1280, 720, L"DX11 Window");
  Render render(window.getHandle());

  window.setRender(&render);
  window.run();

  return 0;
}
