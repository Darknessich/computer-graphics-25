#include "Render.h"
#include <dxgi.h>
#include <cassert>

#pragma comment(lib, "dxgi.lib") // don't work on my computer without these libs
#pragma comment(lib, "d3d11.lib") // uncomment if it doesn't launch

Render::Render(HWND hWnd) {
  HRESULT result;

  // Create a DirectX graphics interface factory.
  IDXGIFactory* pFactory = nullptr;
  result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);

  // Select hardware adapter
  IDXGIAdapter* pSelectedAdapter = NULL;
  if (SUCCEEDED(result))
  {
    IDXGIAdapter* pCurrentAdapter = NULL;
    UINT adapterIdx = 0;
    while (SUCCEEDED(pFactory->EnumAdapters(adapterIdx, &pCurrentAdapter)))
    {
      DXGI_ADAPTER_DESC desc;
      pCurrentAdapter->GetDesc(&desc);

      if (wcscmp(desc.Description, L"Microsoft Basic Render Driver") != 0)
      {
        pSelectedAdapter = pCurrentAdapter;
        break;
      }

      pCurrentAdapter->Release();

      adapterIdx++;
    }
  }
  assert(pSelectedAdapter != NULL);

  // Create DirectX 11 device
  D3D_FEATURE_LEVEL level;
  D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };
  if (SUCCEEDED(result))
  {
    UINT flags = 0;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    result = D3D11CreateDevice(pSelectedAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL,
      flags, levels, 1, D3D11_SDK_VERSION, &device, &level, &context);
    assert(level == D3D_FEATURE_LEVEL_11_0);
    assert(SUCCEEDED(result));
  }

  // Create swapchain
  if (SUCCEEDED(result))
  {
    DXGI_SWAP_CHAIN_DESC scd = { 0 };
    scd.BufferCount = 2;
    scd.BufferDesc.Width = 16;
    scd.BufferDesc.Height = 16;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator = 0;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hWnd;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    scd.Windowed = true;
    scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.Flags = 0;

    result = pFactory->CreateSwapChain(device, &scd, &swapChain);
    assert(SUCCEEDED(result));
  }

  if (SUCCEEDED(result))
  {
    result = createRenderTarget();
  }

  if (pSelectedAdapter != nullptr)
  {
    pSelectedAdapter->Release();
  }

  if (pFactory != nullptr)
  {
    pFactory->Release();
  }
}

Render::~Render() {
  renderTargetView->Release();
  swapChain->Release();
  context->Release();
  device->Release();
}

HRESULT Render::createRenderTarget() {
  ID3D11Texture2D* backBuffer = nullptr;
  HRESULT result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
  
  if (SUCCEEDED(result))
  {
    result = device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);
  }

  if (backBuffer != nullptr)
  {
    backBuffer->Release();
  }

  return result;
}

void Render::resize(UINT width, UINT height) {
  if (swapChain) {
    renderTargetView->Release();
    swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    createRenderTarget();
  }
}

bool Render::render() {
  context->ClearState();

  ID3D11RenderTargetView* views[] = { renderTargetView };
  context->OMSetRenderTargets(1, views, nullptr);

  static const FLOAT BackColor[4] = { 0.33f, 0.33f, 0.67f, 1.0f };
  context->ClearRenderTargetView(renderTargetView, BackColor);

  HRESULT result = swapChain->Present(0, 0);
  assert(SUCCEEDED(result));

  return SUCCEEDED(result);
}