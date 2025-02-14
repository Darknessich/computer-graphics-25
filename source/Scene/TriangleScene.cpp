#include <cstdio>
#include <cassert>
#include <d3dcompiler.h>
#include <string>

#include "utils/utils.h"
#include "TriangleScene.h"

TriangleScene::TriangleScene(const Vertex vertices[3], const USHORT indices[3])
  : m_vertices{ vertices[0], vertices[1], vertices[2] },
  m_indices{ indices[0], indices[1], indices[2] }
{
}

TriangleScene::~TriangleScene()
{
  SafeRelease(m_pInputLayout);
  SafeRelease(m_pPixelShader);
  SafeRelease(m_pVertexShader);
  SafeRelease(m_pVertexBuffer);
  SafeRelease(m_pIndexBuffer);
}

HRESULT TriangleScene::init(ID3D11Device* pDevice)
{
  m_pDevice = pDevice;
  HRESULT result = initVertexBuffer();

  if (SUCCEEDED(result))
  {
    result = initIndexBuffer();
  }

  ID3DBlob* pVertexShaderCode = nullptr;
  if (SUCCEEDED(result))
  {
    result = initVertexShader(&pVertexShaderCode);
  }

  if (SUCCEEDED(result))
  {
    result = initPixelShader();
  }

  if (SUCCEEDED(result))
  {
    result = initInputLayout(pVertexShaderCode);
  }

  SafeRelease(pVertexShaderCode);
  return result;
}

HRESULT TriangleScene::initVertexBuffer()
{
  D3D11_BUFFER_DESC desc = {};
  desc.ByteWidth = sizeof(m_vertices);
  desc.Usage = D3D11_USAGE_IMMUTABLE;
  desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  desc.CPUAccessFlags = 0;
  desc.MiscFlags = 0;
  desc.StructureByteStride = 0;

  D3D11_SUBRESOURCE_DATA data;
  data.pSysMem = &m_vertices;
  data.SysMemPitch = sizeof(m_vertices);
  data.SysMemSlicePitch = 0;

  HRESULT result = m_pDevice->CreateBuffer(&desc, &data, &m_pVertexBuffer);
  assert(SUCCEEDED(result));
  if (SUCCEEDED(result))
  {
    const char name[] = "TriangleSceneVertexBuffer";
    result = m_pVertexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(name), name);
  }

  return result;
}

HRESULT TriangleScene::initIndexBuffer()
{
  D3D11_BUFFER_DESC desc = {};
  desc.ByteWidth = sizeof(m_indices);
  desc.Usage = D3D11_USAGE_IMMUTABLE;
  desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  desc.CPUAccessFlags = 0;
  desc.MiscFlags = 0;
  desc.StructureByteStride = 0;

  D3D11_SUBRESOURCE_DATA data;
  data.pSysMem = &m_indices;
  data.SysMemPitch = sizeof(m_indices);
  data.SysMemSlicePitch = 0;

  HRESULT result = m_pDevice->CreateBuffer(&desc, &data, &m_pIndexBuffer);
  
  assert(SUCCEEDED(result));
  if (SUCCEEDED(result))
  {
    const char name[] = "TriangleSceneIndexBuffer";
    result = m_pIndexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(name), name);
  }

  return result;
}

HRESULT TriangleScene::initVertexShader(ID3DBlob** ppCode)
{
  static const LPCWSTR path = L"source/shaders/TriangleScene.vs";

  HRESULT result = CompileShader(path, "vs", "vs_5_0", ppCode);

  if (SUCCEEDED(result))
  {
    result = m_pDevice->CreateVertexShader((*ppCode)->GetBufferPointer(), (*ppCode)->GetBufferSize(), nullptr, &m_pVertexShader);
  }

  if (SUCCEEDED(result))
  {
    const char name[] = "TriangleSceneVertexShader";
    result = result = m_pVertexShader->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(name), name);
  }

  return result;
}

HRESULT TriangleScene::initPixelShader()
{
  static const LPCWSTR path = L"source/shaders/TriangleScene.ps";

  ID3DBlob* pCode = nullptr;
  HRESULT result = CompileShader(path, "ps", "ps_5_0", &pCode);

  if (SUCCEEDED(result))
  {
    result = m_pDevice->CreatePixelShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), nullptr, &m_pPixelShader);
  }

  if (SUCCEEDED(result))
  {
    const char name[] = "TriangleScenePixelShader";;
    result = result = m_pPixelShader->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(name), name);
  }

  return result;
}

HRESULT TriangleScene::initInputLayout(ID3DBlob* pVertexShaderCode)
{
  static const D3D11_INPUT_ELEMENT_DESC InputDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
  };

  HRESULT result = m_pDevice->CreateInputLayout(InputDesc, 2, pVertexShaderCode->GetBufferPointer(), pVertexShaderCode->GetBufferSize(), &m_pInputLayout);
  
  if (SUCCEEDED(result))
  {
    const char name[] = "TriangleSceneInputLayout";;
    result = result = m_pInputLayout->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(name), name);
  }

  return result;
}

void TriangleScene::draw(ID3D11DeviceContext* pDeviceContext)
{
  ID3D11Buffer* vertexBuffers[] = { m_pVertexBuffer };
  UINT strides[] = { 16 }, offsets[] = { 0 };
  
  pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
  pDeviceContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
  pDeviceContext->IASetInputLayout(m_pInputLayout);
  pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
  pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
  pDeviceContext->DrawIndexed(3, 0, 0);
}