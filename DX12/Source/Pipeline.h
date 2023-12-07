#pragma once
#include "GraphicDevice.h"
class Pipeline {
public:
  Pipeline();
  ~Pipeline();
  bool Create(GraphicDevice *device, ID3D12RootSignature *rootSignature);
  ID3D12PipelineState *Get() const { return pipelineState.Get(); }
private:
  Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
};
