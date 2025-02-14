#pragma once

#include <d3d11.h>

class IScene {
public:
  virtual HRESULT init(ID3D11Device* pDevice) = 0;
  virtual void draw(ID3D11DeviceContext* pDeviceContext) = 0;
  virtual ~IScene() = default;
};