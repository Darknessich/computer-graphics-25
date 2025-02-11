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

  Window window;

  if (!window.init(hInstance, L"CG25-DX11 01-Window"))
  {
    return FALSE;
  }

  Render render(window.getHandle());
  window.setRender(&render);
  window.show(nCmdShow, 1280, 720);
  return window.run();
}
