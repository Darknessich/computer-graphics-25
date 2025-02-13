#pragma once

#include "IScene.h"

class TriangleScene : public IScene {
public:
  struct Vertex {
    float x, y, z;
    COLORREF color;
  };

  TriangleScene(const Vertex vertices[3], const USHORT indices[3]);
  ~TriangleScene();
  
  HRESULT init(ID3D11Device* pDevice) override;
  void draw(ID3D11DeviceContext* pDeviceContext) override;

private:
  HRESULT initVertexBuffer();
  HRESULT initIndexBuffer();

  HRESULT initVertexShader(ID3DBlob** ppCode);
  HRESULT initPixelShader();
  HRESULT initInputLayout(ID3DBlob* pVertexShaderCode);

  const Vertex m_vertices[3];
  const USHORT m_indices[3];
  ID3D11Device* m_pDevice{ nullptr };

  ID3D11Buffer* m_pVertexBuffer{ nullptr };
  ID3D11Buffer* m_pIndexBuffer{ nullptr };

  ID3D11VertexShader* m_pVertexShader{ nullptr };
  ID3D11PixelShader* m_pPixelShader{ nullptr };
  ID3D11InputLayout* m_pInputLayout{ nullptr };
};