#pragma once

#include <d3d11.h>

class Render {
public:
  ~Render();
  bool init(HWND hWnd);
  bool render();
  bool resize(UINT width, UINT height);

private:
  HRESULT createDevice(IDXGIAdapter* pAdapter);
  HRESULT createSwapChain(HWND hWnd, IDXGIFactory* pFactory);
  HRESULT createRenderTarget();

  ID3D11Device* m_device{ nullptr };
  ID3D11DeviceContext* m_deviceContext{ nullptr };
  IDXGISwapChain* m_swapChain{ nullptr };
  ID3D11RenderTargetView* m_renderTargetView{ nullptr };
};