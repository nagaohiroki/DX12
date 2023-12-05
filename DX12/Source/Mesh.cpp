#include "Mesh.h"
#include <algorithm>

Mesh::Mesh() {}

Mesh::~Mesh() {}

void Mesh::Create(GraphicDevice *device) {
  triangles[0] = DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f);
  triangles[1] = DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f);
  triangles[2] = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
  D3D12_HEAP_PROPERTIES heapProps = {};
  heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
  heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
  heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

  D3D12_RESOURCE_DESC resourceDesc = {};
  resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
  resourceDesc.Width = sizeof(triangles);
  resourceDesc.Height = 0;
  resourceDesc.DepthOrArraySize = 1;
  resourceDesc.MipLevels = 1;
  resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
  resourceDesc.SampleDesc.Count = 1;
  resourceDesc.SampleDesc.Quality = 0;
  resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
  resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

  device->GetDevice()->CreateCommittedResource(
      &heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc,
      D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer));
  vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
  vertexBufferView.StrideInBytes = static_cast<UINT>(resourceDesc.Width);
  vertexBufferView.SizeInBytes = sizeof(DirectX::XMFLOAT3);

  DirectX::XMFLOAT3 *pData = nullptr;
  vertexBuffer->Map(0, nullptr, reinterpret_cast<void **>(&pData));
  std::copy(std::begin(triangles), std::end(triangles), pData);
  vertexBuffer->Unmap(0, nullptr);
}

void Mesh::DrawInstanced(GraphicDevice *device) 
{
  device->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
  device->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}
