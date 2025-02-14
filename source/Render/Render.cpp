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
    result = pScene->init(m_pDevice);
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
    D3D11_SDK_VERSION, &m_pDevice, &level, &m_pDeviceContext
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

  return pFactory->CreateSwapChain(m_pDevice, &scd, &m_pSwapChain);
}

Render::~Render() {
  delete m_pScene;

  SafeRelease(m_pRenderTargetView);
  SafeRelease(m_pSwapChain);
  SafeRelease(m_pDeviceContext);

#ifdef _DEBUG
  if (m_pDevice != nullptr)
  {
    ID3D11Debug* pDebug = nullptr;
    HRESULT result = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&pDebug);
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

  SafeRelease(m_pDevice);
}

HRESULT Render::createRenderTarget() {
  ID3D11Texture2D* pBackBuffer = nullptr;
  HRESULT result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
  
  if (SUCCEEDED(result))
  {
    result = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
  }

  SafeRelease(pBackBuffer);
  return result;
}

bool Render::resize(UINT width, UINT height) {
  if (m_pSwapChain == nullptr)
  {
    return true;
  }

  SafeRelease(m_pRenderTargetView);
  HRESULT result = m_pSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
  
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
  m_pDeviceContext->ClearState();

  ID3D11RenderTargetView* views[] = { m_pRenderTargetView };
  m_pDeviceContext->OMSetRenderTargets(1, views, nullptr);

  static const FLOAT BackColor[4] = { 0.12f, 0.14f, 0.38f, 1.0f };
  m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, BackColor);

  setupViewport();
  if (m_pScene != nullptr)
  {
    m_pScene->draw(m_pDeviceContext);
  }

  HRESULT result = m_pSwapChain->Present(0, 0);

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
  m_pDeviceContext->RSSetViewports(1, &viewport);

  D3D11_RECT rect;
  rect.left = 0;
  rect.top = 0;
  rect.right = m_width;
  rect.bottom = m_height;
  m_pDeviceContext->RSSetScissorRects(1, &rect);
}