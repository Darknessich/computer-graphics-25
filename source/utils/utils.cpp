#include "utils.h"
#include <d3dcompiler.h>
#include <cassert>

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

HRESULT ReadShaderData(LPCWSTR path, LPCVOID* ppData, SIZE_T* pSize)
{
  FILE* pFile = nullptr;
  _wfopen_s(&pFile, path, L"rb");
  assert(pFile != nullptr);
  if (pFile == nullptr)
  {
    return E_FAIL;
  }

  fseek(pFile, 0, SEEK_END);
  long long size = _ftelli64(pFile);
  fseek(pFile, 0, SEEK_SET);

  char* data = new char[size + 1];
  size_t rd = fread(data, 1, size, pFile);
  assert(rd != (size_t)size);
  if (rd != (size_t)size)
  {
    return E_FAIL;
  }

  *ppData = (LPCVOID)data;
  *pSize = size;
  fclose(pFile);
  return S_OK;
}

HRESULT CompileShader(LPCWSTR path, LPCSTR entryPoint, LPCSTR target, ID3DBlob** ppCode)
{
  LPCVOID data = nullptr;
  SIZE_T size = 0;
  HRESULT result = ReadShaderData(path, &data, &size);

  ID3DBlob* pErrMsg = nullptr;

  UINT flags = 0;
#ifdef _DEBUG
  flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif 

  if (SUCCEEDED(result))
  {
    result = D3DCompile(data, size, wstr2str(path).c_str(), nullptr, nullptr, entryPoint, target, flags, 0, ppCode, &pErrMsg);
  }

  if (FAILED(result) && pErrMsg != nullptr)
  {
    OutputDebugStringA((LPCSTR)pErrMsg->GetBufferPointer());
  }

  SafeRelease(pErrMsg);
  return result;
}
