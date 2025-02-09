#pragma once

#include <d3d11.h>

class Render {
public:
  Render(HWND hWnd);
  ~Render();
  bool render();
  void resize(UINT width, UINT height);

private:
  HRESULT createRenderTarget();

  ID3D11Device* device;
  ID3D11DeviceContext* context;
  IDXGISwapChain* swapChain;
  ID3D11RenderTargetView* renderTargetView;
};