#include "Pipeline.h"
#include <vector>
Pipeline::Pipeline() {}
Pipeline::~Pipeline() {}
bool Pipeline::Create(GraphicDevice *device,
                      ID3D12RootSignature *rootSignature) {
  std::vector<ID3DBlob *> blobs;
  std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;
  D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
  // vertex shader
  pipelineDesc.VS.pShaderBytecode = blobs[0]->GetBufferPointer();
  pipelineDesc.VS.BytecodeLength = blobs[0]->GetBufferSize();
  // hal shader
  pipelineDesc.HS.pShaderBytecode = blobs[1]->GetBufferPointer();
  pipelineDesc.HS.BytecodeLength = blobs[1]->GetBufferSize();
  // domain shader
  pipelineDesc.DS.pShaderBytecode = blobs[2]->GetBufferPointer();
  pipelineDesc.DS.BytecodeLength = blobs[2]->GetBufferSize();
  // geometry shader
  pipelineDesc.GS.pShaderBytecode = blobs[3]->GetBufferPointer();
  pipelineDesc.GS.BytecodeLength = blobs[3]->GetBufferSize();
  // pixel shader
  pipelineDesc.PS.pShaderBytecode = blobs[4]->GetBufferPointer();
  pipelineDesc.PS.BytecodeLength = blobs[4]->GetBufferSize();
  // mode
  pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
  pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
  pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
  // depth
  bool isDepth = true;
  bool isDepthMask = true;
  pipelineDesc.RasterizerState.DepthClipEnable = isDepth;
  pipelineDesc.DepthStencilState.DepthEnable = isDepth;
  if (isDepth) {
    pipelineDesc.DepthStencilState.StencilEnable = !isDepth;
    pipelineDesc.DepthStencilState.DepthWriteMask =
        isDepthMask ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
  }
  pipelineDesc.BlendState.AlphaToCoverageEnable = false;
  pipelineDesc.BlendState.IndependentBlendEnable = false;
  // blend
  D3D12_RENDER_TARGET_BLEND_DESC blendDesc = {};
  blendDesc.BlendEnable = true;
  blendDesc.LogicOpEnable = false;
  blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
  blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
  blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
  blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
  blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
  blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
  blendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
  blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
  pipelineDesc.BlendState.RenderTarget[0] = blendDesc;
  // input
  pipelineDesc.InputLayout.pInputElementDescs = inputElementDescs.data();
  pipelineDesc.InputLayout.NumElements = static_cast<UINT>(inputElementDescs.size());
  // topology
  pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
  // RVT
  int rtvCount = 3;
  pipelineDesc.NumRenderTargets = rtvCount;
  pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
  pipelineDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
  pipelineDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
  pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
  // sample
  pipelineDesc.SampleDesc.Count = 1;
  pipelineDesc.pRootSignature = rootSignature;
  // pipeline
  if (FAILED(device->GetDevice()->CreateGraphicsPipelineState(
          &pipelineDesc, IID_PPV_ARGS(&pipelineState)))) {
    return false;
  }
  return true;
}
