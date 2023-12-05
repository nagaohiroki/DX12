#pragma once
#include <array>
#include <wrl/client.h>
#include "DirectXMath.h"
#include "GraphicDevice.h"
class Mesh {
public:
  Mesh();
  virtual ~Mesh();
  void Create(GraphicDevice* device);
  void DrawInstanced(GraphicDevice* device);
private:
  std::array<DirectX::XMFLOAT3, 3> triangles;
  Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
  Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
  D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
  D3D12_INDEX_BUFFER_VIEW indexBufferView;
};
