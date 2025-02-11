#include "Render.h"
#include <dxgi.h>
#include <cassert>

#pragma comment(lib, "dxgi.lib") // don't work on my computer without these libs
#pragma comment(lib, "d3d11.lib") // uncomment if it doesn't launch

IDXGIAdapter* selectHardwareAdapter(IDXGIFactory* pFactory)
{
  UINT adapterIdx = 0;
  IDXGIAdapter* pSelectedAdapter = nullptr;
  IDXGIAdapter* pCurrentAdapter = nullptr;
  
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

  return pSelectedAdapter;
}

bool Render::init(HWND hWnd) 
{
  IDXGIFactory* pFactory{ nullptr };
  IDXGIAdapter* pAdapter{ nullptr };
  HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);
  
  if (SUCCEEDED(result))
  {
    pAdapter = selectHardwareAdapter(pFactory);
    result = createDevice(pAdapter);
  }

  if (SUCCEEDED(result))
  {
    result = createSwapChain(hWnd, pFactory);
  }

  if (SUCCEEDED(result))
  {
    result = createRenderTarget();
  }

  if (pAdapter != nullptr)
  {
    pAdapter->Release();
  }

  if (pFactory != nullptr)
  {
    pFactory->Release();
  }

  return SUCCEEDED(result);
}

HRESULT Render::createDevice(IDXGIAdapter* pAdapter)
{
  assert(pAdapter != nullptr);

  D3D_FEATURE_LEVEL level;
  D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };

  UINT flags = 0;
#ifdef _DEBUG
  flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
  HRESULT result = D3D11CreateDevice(
    pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, flags, levels, 1, 
    D3D11_SDK_VERSION, &m_device, &level, &m_deviceContext
  );
  assert(level == D3D_FEATURE_LEVEL_11_0);
  return result;
}

HRESULT Render::createSwapChain(HWND hWnd, IDXGIFactory* pFactory)
{
  assert(pFactory != nullptr);

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

  return pFactory->CreateSwapChain(m_device, &scd, &m_swapChain);
}

Render::~Render() {
  IUnknown* members[] = { m_device, m_deviceContext, m_renderTargetView, m_swapChain };

  for (auto member : members)
  {
    if (member != nullptr)
    {
      member->Release();
    }
  }
}

HRESULT Render::createRenderTarget() {
  ID3D11Texture2D* backBuffer = nullptr;
  HRESULT result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
  
  if (SUCCEEDED(result))
  {
    result = m_device->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
  }

  if (backBuffer != nullptr)
  {
    backBuffer->Release();
  }

  return result;
}

bool Render::resize(UINT width, UINT height) {
  if (m_swapChain == nullptr)
  {
    return true;
  }

  if (m_renderTargetView != nullptr)
  {
    m_renderTargetView->Release();
  }

  HRESULT result = m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
  return SUCCEEDED(result) && SUCCEEDED(createRenderTarget());
}

bool Render::render() {
  m_deviceContext->ClearState();

  ID3D11RenderTargetView* views[] = { m_renderTargetView };
  m_deviceContext->OMSetRenderTargets(1, views, nullptr);

  static const FLOAT BackColor[4] = { 0.10f, 0.41f, 0.88f, 1.0f };
  m_deviceContext->ClearRenderTargetView(m_renderTargetView, BackColor);

  HRESULT result = m_swapChain->Present(0, 0);

  return SUCCEEDED(result);
}