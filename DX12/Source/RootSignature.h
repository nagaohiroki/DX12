#pragma once
#include "GraphicDevice.h"
#include <vector>
class RootSignature {
public:
  enum class RangeType {
    ConstantBufferView,
    ShaderResourceView,
    UnorderedAccessView
  };
  RootSignature();
  ~RootSignature();
  bool Create(GraphicDevice *device, std::vector<RangeType> &ranges);
  ID3D12RootSignature *Get() const { return rootSignature.Get(); }

private:
  Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
  Microsoft::WRL::ComPtr<ID3DBlob> rootBlob;
  void CreateStateSampler(D3D12_STATIC_SAMPLER_DESC &sampler,
                          D3D12_TEXTURE_ADDRESS_MODE mode, D3D12_FILTER filter,
                          int count);
};
