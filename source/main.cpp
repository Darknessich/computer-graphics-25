#include "Window/Window.h"
#include "Render/Render.h"
#include "Scene/TriangleScene.h"

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
  Render render;

  if (!window.init(hInstance, L"CG25-DX11 02-Triangle"))
  {
    return FALSE;
  }

  TriangleScene::Vertex vertices[3] = {
      {-0.5f, -0.5f, 0.0f, RGB(255, 0, 0)},
      { 0.5f, -0.5f, 0.0f, RGB(0, 255, 0)},
      { 0.0f,  0.5f, 0.0f, RGB(0, 0, 255)}
  };
  USHORT indices[3] = {0U, 2U, 1U};
  IScene* pScene = new TriangleScene(vertices, indices);
  if (!render.init(window.getHandle(), pScene))
  {
    return FALSE;
  }

  window.setRender(&render);
  window.show(nCmdShow, 1280, 720);
  return window.run();
}
