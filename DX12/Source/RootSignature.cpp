#include "RootSignature.h"
#include "Logger.h"
RootSignature::RootSignature() {}
RootSignature::~RootSignature() {}

bool RootSignature::Create(GraphicDevice *device,
                           std::vector<RangeType> &ranges) {
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

void RootSignature::CreateStateSampler(D3D12_STATIC_SAMPLER_DESC &sampler,
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
