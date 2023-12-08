#include "Pipeline.h"
#include "Logger.h"
#include <d3dcompiler.h>
Pipeline::Pipeline() {}
Pipeline::~Pipeline() {}
bool Pipeline::Create(GraphicDevice *device) {
  const std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs = {
      D3D12_INPUT_ELEMENT_DESC{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
                               D3D12_APPEND_ALIGNED_ELEMENT,
                               D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
      D3D12_INPUT_ELEMENT_DESC{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
                               D3D12_APPEND_ALIGNED_ELEMENT,
                               D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
      D3D12_INPUT_ELEMENT_DESC{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
                               D3D12_APPEND_ALIGNED_ELEMENT,
                               D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
      D3D12_INPUT_ELEMENT_DESC{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0,
                               D3D12_APPEND_ALIGNED_ELEMENT,
                               D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
      D3D12_INPUT_ELEMENT_DESC{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
                               D3D12_APPEND_ALIGNED_ELEMENT,
                               D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
      D3D12_INPUT_ELEMENT_DESC{"SKININDEX", 0, DXGI_FORMAT_R16G16B16A16_UINT, 0,
                               D3D12_APPEND_ALIGNED_ELEMENT,
                               D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
      D3D12_INPUT_ELEMENT_DESC{"SKINWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
                               0, D3D12_APPEND_ALIGNED_ELEMENT,
                               D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
  };
  CreateShader();
  D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
  // vertex shader
  pipelineDesc.VS.pShaderBytecode =
      shaders[ShaderType::Vertex]->GetBufferPointer();
  pipelineDesc.VS.BytecodeLength = shaders[ShaderType::Vertex]->GetBufferSize();
  // hal shader
  pipelineDesc.HS.pShaderBytecode =
      shaders[ShaderType::Hull]->GetBufferPointer();
  pipelineDesc.HS.BytecodeLength = shaders[ShaderType::Hull]->GetBufferSize();
  // domain shader
  pipelineDesc.DS.pShaderBytecode =
      shaders[ShaderType::Domain]->GetBufferPointer();
  pipelineDesc.DS.BytecodeLength = shaders[ShaderType::Domain]->GetBufferSize();
  // geometry shader
  pipelineDesc.GS.pShaderBytecode =
      shaders[ShaderType::Geometry]->GetBufferPointer();
  pipelineDesc.GS.BytecodeLength =
      shaders[ShaderType::Geometry]->GetBufferSize();
  // pixel shader
  pipelineDesc.PS.pShaderBytecode =
      shaders[ShaderType::Pixel]->GetBufferPointer();
  pipelineDesc.PS.BytecodeLength = shaders[ShaderType::Pixel]->GetBufferSize();
  // input
  pipelineDesc.InputLayout.pInputElementDescs = inputElementDescs.data();
  pipelineDesc.InputLayout.NumElements =
      static_cast<UINT>(inputElementDescs.size());
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
  pipelineDesc.InputLayout.NumElements =
      static_cast<UINT>(inputElementDescs.size());
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
  CreateRootSignature(device, {});
  pipelineDesc.pRootSignature = rootSignature.Get();
  // pipeline
  if (FAILED(device->GetDevice()->CreateGraphicsPipelineState(
          &pipelineDesc, IID_PPV_ARGS(&pipelineState)))) {
    return false;
  }
  return true;
}
bool Pipeline::CreateRootSignature(GraphicDevice *device,
                                   const std::vector<RangeType> &ranges) {
  size_t rangesCount = ranges.size();
  std::vector<D3D12_ROOT_PARAMETER> rootParams(rangesCount);
  std::vector<D3D12_DESCRIPTOR_RANGE> descRanges(rangesCount);
  int constantBufferViewCount = 0;
  int shaderResourceViewCount = 0;
  int unorderedAccessViewCount = 0;
  int sampleCount = 0;
  for (size_t i = 0; i < rangesCount; i++) {
    descRanges[i] = {};
    switch (ranges[i]) {
    case RangeType::ConstantBufferView:
      descRanges[i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
      descRanges[i].NumDescriptors = 1;
      descRanges[i].BaseShaderRegister = constantBufferViewCount;
      descRanges[i].OffsetInDescriptorsFromTableStart =
          D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
      rootParams[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
      rootParams[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
      rootParams[i].DescriptorTable.pDescriptorRanges = &descRanges[i];
      rootParams[i].DescriptorTable.NumDescriptorRanges = 1;
      constantBufferViewCount++;

      break;
    case RangeType::ShaderResourceView:
      descRanges[i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
      descRanges[i].NumDescriptors = 1;
      descRanges[i].BaseShaderRegister = shaderResourceViewCount;
      descRanges[i].OffsetInDescriptorsFromTableStart =
          D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
      rootParams[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
      rootParams[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
      rootParams[i].DescriptorTable.pDescriptorRanges = &descRanges[i];
      rootParams[i].DescriptorTable.NumDescriptorRanges = 1;

      shaderResourceViewCount++;
      break;
    case RangeType::UnorderedAccessView:
      descRanges[i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
      descRanges[i].NumDescriptors = 1;
      descRanges[i].BaseShaderRegister = unorderedAccessViewCount;
      descRanges[i].OffsetInDescriptorsFromTableStart =
          D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
      rootParams[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
      rootParams[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
      rootParams[i].DescriptorTable.pDescriptorRanges = &descRanges[i];
      rootParams[i].DescriptorTable.NumDescriptorRanges = 1;
      unorderedAccessViewCount++;
      break;
    }
  }
  D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
  if (shaderResourceViewCount > 0) {
    std::array<D3D12_STATIC_SAMPLER_DESC, 4> samplerDescs = {};
    CreateStateSampler(samplerDescs[0], D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                       D3D12_FILTER_MIN_MAG_MIP_POINT, 0);
    CreateStateSampler(samplerDescs[1], D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                       D3D12_FILTER_MIN_MAG_MIP_POINT, 1);
    CreateStateSampler(samplerDescs[2], D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                       D3D12_FILTER_MIN_MAG_MIP_LINEAR, 2);
    CreateStateSampler(samplerDescs[3], D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                       D3D12_FILTER_MIN_MAG_MIP_LINEAR, 3);
    rootSignatureDesc.pStaticSamplers = samplerDescs.data();
    rootSignatureDesc.NumStaticSamplers =
        static_cast<UINT>(samplerDescs.size());
  }
  rootSignatureDesc.pParameters = rootParams.data();
  rootSignatureDesc.pStaticSamplers = nullptr;
  rootSignatureDesc.Flags =
      D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
  rootSignatureDesc.NumParameters = static_cast<UINT>(rootParams.size());
  ID3DBlob *error = nullptr;
  if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc,
                                         D3D_ROOT_SIGNATURE_VERSION_1_0,
                                         &rootBlob, &error))) {
    Logger::Log(_T("failed to serialize root signature"));
    return false;
  }
  if (FAILED(device->GetDevice()->CreateRootSignature(
          0, rootBlob->GetBufferPointer(), rootBlob->GetBufferSize(),
          IID_PPV_ARGS(&rootSignature)))) {
    Logger::Log(_T("failed to create root signature"));
    return false;
  }
  return true;
}

void Pipeline::CreateStateSampler(D3D12_STATIC_SAMPLER_DESC &sampler,
                                  D3D12_TEXTURE_ADDRESS_MODE addressMode,
                                  D3D12_FILTER filter, int count) {
  sampler = {};
  sampler.AddressU = addressMode;
  sampler.AddressV = addressMode;
  sampler.AddressW = addressMode;
  sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
  sampler.Filter = filter;
  sampler.MaxLOD = D3D12_FLOAT32_MAX;
  sampler.MinLOD = 0.0f;
  sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
  sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
  sampler.MaxAnisotropy = 16;
  sampler.ShaderRegister = count;
}
void Pipeline::CreateShader() {
  shaders.emplace(ShaderType::Vertex,
                  LoadShader(L"Asset\\Shader\\VS.hlsl", "vs_5_0"));
  shaders.emplace(ShaderType::Hull,
                  LoadShader(L"Asset\\Shader\\HS.hlsl", "hs_5_0"));
  shaders.emplace(ShaderType::Domain,
                  LoadShader(L"Asset\\Shader\\DS.hlsl", "ds_5_0"));
  shaders.emplace(ShaderType::Geometry,
                  LoadShader(L"Asset\\Shader\\GS.hlsl", "gs_5_0"));
  shaders.emplace(ShaderType::Pixel,
                  LoadShader(L"Asset\\Shader\\PS.hlsl", "ps_5_0"));
}

ID3DBlob *Pipeline::LoadShader(const std::wstring &fileName,
                               const std::string &content) {
  ID3DBlob *error = nullptr;
  ID3DBlob *shader = nullptr;
  if (FAILED(D3DCompileFromFile(
          fileName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
          content.c_str(), D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
          &shader, &error))) {
    return nullptr;
  }
  return shader;
}
