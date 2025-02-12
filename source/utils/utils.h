#pragma once

#include <d3d11.h>
#include <string>

inline void SafeRelease(IUnknown* p)
{
  if (p != nullptr)
  {
    p->Release();
  }
}

std::string wstr2str(const std::wstring& wstr);

IDXGIAdapter* SelectHardwareAdapter(IDXGIFactory* pFactory);

HRESULT ReadShaderData(LPCWSTR path, LPCVOID* ppData, SIZE_T* pSize);
HRESULT CompileShader(LPCWSTR path, LPCSTR entryPoint, LPCSTR target, ID3DBlob** ppCode);