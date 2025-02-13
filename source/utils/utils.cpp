#include "utils.h"
#include <d3dcompiler.h>
#include <cassert>
#include <cstring>

std::string wstr2str(const std::wstring& wstr)
{
  std::string str(wstr.length(), '\0');
  for (int i = 0; i < wstr.length(); ++i)
  {
    str[i] = static_cast<char>(wstr[i]);
  }
  return str;
}

IDXGIAdapter* SelectHardwareAdapter(IDXGIFactory* pFactory)
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

HRESULT CompileShader(LPCWSTR path, LPCSTR entryPoint, LPCSTR target, ID3DBlob** ppCode)
{
  ID3DBlob* pErrMsg = nullptr;

  UINT flags = 0;
#ifdef _DEBUG
  flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif 

  HRESULT result = D3DCompileFromFile(path, nullptr, nullptr, entryPoint, target, flags, 0, ppCode, &pErrMsg);

  if (FAILED(result) && pErrMsg != nullptr)
  {
    OutputDebugStringA((LPCSTR)pErrMsg->GetBufferPointer());
  }

  SafeRelease(pErrMsg);
  return result;
}
