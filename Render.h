#pragma once

#include <d3d11.h>

class Render {
public:
  Render(HWND hWnd);
  ~Render();
  void render();

private:
  ID3D11Device* device;
  ID3D11DeviceContext* context;
  IDXGISwapChain* swapChain;
  ID3D11RenderTargetView* renderTargetView;
};