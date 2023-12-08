#pragma once
#include "GraphicDevice.h"
#include "tstring.h"
#include <memory>
#include <unordered_map>
#include <vector>
enum class ShaderType {
  Vertex,
  Hull,
  Domain,
  Geometry,
  Pixel,
};
enum class RangeType {
  ConstantBufferView,
  ShaderResourceView,
  UnorderedAccessView
};
class Pipeline {
public:
  Pipeline();
  ~Pipeline();
  bool Create(GraphicDevice *device);
  ID3D12PipelineState *Get() const { return pipelineState.Get(); }

private:
  Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
  std::unordered_map<ShaderType, Microsoft::WRL::ComPtr<ID3DBlob>> shaders;

  Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
  Microsoft::WRL::ComPtr<ID3DBlob> rootBlob;

  bool CreateRootSignature(GraphicDevice *device, const std::vector<RangeType> &ranges);
  void CreateStateSampler(D3D12_STATIC_SAMPLER_DESC &sampler,
                          D3D12_TEXTURE_ADDRESS_MODE mode, D3D12_FILTER filter,
                          int count);
  void CreateShader();
  ID3DBlob *LoadShader(const std::wstring &fileName, const std::string &content);

};

