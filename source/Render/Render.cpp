#include <dxgi.h>
#include <cassert>

#include "utils/utils.h"
#include "Render.h"

bool Render::init(HWND hWnd, IScene* pScene) 
{
  IDXGIFactory* pFactory{ nullptr };
  IDXGIAdapter* pAdapter{ nullptr };
  HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);
  
  if (SUCCEEDED(result))
  {
    pAdapter = SelectHardwareAdapter(pFactory);
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

  if (SUCCEEDED(result) && pScene != nullptr)
  {
    result = pScene->init(m_device);
  }

  m_pScene = pScene;
  SafeRelease(pAdapter);
  SafeRelease(pFactory);
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
  delete m_pScene;

  SafeRelease(m_renderTargetView);
  SafeRelease(m_swapChain);
  SafeRelease(m_deviceContext);

#ifdef _DEBUG
  if (m_device != nullptr)
  {
    ID3D11Debug* pDebug = nullptr;
    HRESULT result = m_device->QueryInterface(__uuidof(ID3D11Debug), (void**)&pDebug);
    assert(SUCCEEDED(result));
    if (pDebug != nullptr)
    {
      if (pDebug->AddRef() != 3) // ID3D11Device && ID3D11Debug && after AddRef()
      {
        pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
      }
      pDebug->Release();

      SafeRelease(pDebug);
    }
  }
#endif // _DEBUG

  SafeRelease(m_device);
}

HRESULT Render::createRenderTarget() {
  ID3D11Texture2D* backBuffer = nullptr;
  HRESULT result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
  
  if (SUCCEEDED(result))
  {
    result = m_device->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
  }

  SafeRelease(backBuffer);
  return result;
}

bool Render::resize(UINT width, UINT height) {
  if (m_swapChain == nullptr)
  {
    return true;
  }

  SafeRelease(m_renderTargetView);
  HRESULT result = m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
  
  if (SUCCEEDED(result))
  {
    result = createRenderTarget();
  }

  if (SUCCEEDED(result))
  {
    m_width = width;
    m_height = height;
  }

  return SUCCEEDED(result);
}

bool Render::render() {
  m_deviceContext->ClearState();

  ID3D11RenderTargetView* views[] = { m_renderTargetView };
  m_deviceContext->OMSetRenderTargets(1, views, nullptr);

  static const FLOAT BackColor[4] = { 0.12f, 0.14f, 0.38f, 1.0f };
  m_deviceContext->ClearRenderTargetView(m_renderTargetView, BackColor);

  setupViewport();
  if (m_pScene != nullptr)
  {
    m_pScene->draw(m_deviceContext);
  }

  HRESULT result = m_swapChain->Present(0, 0);

  return SUCCEEDED(result);
}

void Render::setupViewport()
{
  D3D11_VIEWPORT viewport;
  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;
  viewport.Width = (FLOAT)m_width;
  viewport.Height = (FLOAT)m_height;
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  m_deviceContext->RSSetViewports(1, &viewport);

  D3D11_RECT rect;
  rect.left = 0;
  rect.top = 0;
  rect.right = m_width;
  rect.bottom = m_height;
  m_deviceContext->RSSetScissorRects(1, &rect);
}