#include "GraphicDevice.h"
#include "Logger.h"
RtvHeap::RtvHeap() {}
RtvHeap::~RtvHeap() {}
bool RtvHeap::Create(ID3D12Device *device, int numDescriptors) {
  D3D12_DESCRIPTOR_HEAP_DESC desc = {};
  desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  desc.NumDescriptors = numDescriptors;
  desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  desc.NodeMask = 0;
  if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtvHeap)))) {
    Logger::Log(_T("failed to create descriptor heap"));
    return false;
  }
  rtvDescriptorSize =
      device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
  return true;
}
int RtvHeap::CreateRTV(ID3D12Device *device, ID3D12Resource *resource) {
  D3D12_CPU_DESCRIPTOR_HANDLE handle =
      rtvHeap->GetCPUDescriptorHandleForHeapStart();
  handle.ptr += (UINT64)nextRegistNumber * rtvDescriptorSize;

  D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
  rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
  device->CreateRenderTargetView(resource, &rtvDesc, handle);
  return nextRegistNumber++;
}
D3D12_CPU_DESCRIPTOR_HANDLE RtvHeap::GetHandle(int index) const {
  D3D12_CPU_DESCRIPTOR_HANDLE handle =
      rtvHeap->GetCPUDescriptorHandleForHeapStart();
  handle.ptr += index * rtvDescriptorSize;
  return handle;
}
GraphicDevice::GraphicDevice() {}
GraphicDevice::~GraphicDevice() {}
bool GraphicDevice::Create(HWND hWnd, UINT width, UINT height) {
  if (!CreateFactory()) {
    return false;
  }
#if _DEBUG
  EnableDebugLayer();
#endif
  if (!CreateDevice()) {
    return false;
  }
  if (!CreateCommandList()) {
    return false;
  }
  if (!CreateSwapChain(hWnd, width, height)) {
    return false;
  }
  if (!rtv.Create(device.Get(), 100)) {
    return false;
  }
  if (!CreateSwapChainRTV()) {
    return false;
  }
  if (!CreateFence()) {
    return false;
  }
  return true;
}

bool GraphicDevice::CreateFactory() {
  UINT flagDXGI = 0;

#ifdef _DEBUG
  flagDXGI = DXGI_CREATE_FACTORY_DEBUG;
#endif
  if (FAILED(
          CreateDXGIFactory2(flagDXGI, IID_PPV_ARGS(factory.GetAddressOf())))) {
    Logger::Log(_T("failed to create factory"));
    return false;
  }
  Logger::Log(_T("success to create factory"));
  return true;
}

bool GraphicDevice::CreateDevice() {
  D3D_FEATURE_LEVEL levels[] = {
      D3D_FEATURE_LEVEL_12_1,
      D3D_FEATURE_LEVEL_12_0,
      D3D_FEATURE_LEVEL_11_1,
      D3D_FEATURE_LEVEL_11_0,
  };
  Microsoft::WRL::ComPtr<IDXGIAdapter> adapter = nullptr;
  for (UINT i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND;
       ++i) {
    DXGI_ADAPTER_DESC desc;
    adapter->GetDesc(&desc);
    for (D3D_FEATURE_LEVEL lv : levels) {
      if (D3D12CreateDevice(adapter.Get(), lv, IID_PPV_ARGS(&device)) == S_OK) {
        Logger::Log(_T("success to create device"));
        Logger::Log(desc.Description);
        return true;
      }
    }
  }
  Logger::Log(_T("failed to create device"));
  return false;
}
bool GraphicDevice::CreateCommandList() {
  if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                            IID_PPV_ARGS(&commandAllocator)))) {
    Logger::Log(_T("failed to create command allocator"));
    return false;
  }
  if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                       commandAllocator.Get(), nullptr,
                                       IID_PPV_ARGS(&commandList)))) {
    Logger::Log(_T("failed to create command list"));
    return false;
  }
  D3D12_COMMAND_QUEUE_DESC queueDesc = {};
  queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
  queueDesc.NodeMask = 0;
  queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
  queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
  if (FAILED(device->CreateCommandQueue(&queueDesc,
                                        IID_PPV_ARGS(&commandQueue)))) {
    Logger::Log(_T("failed to create command queue"));
    return false;
  }
  return true;
}

bool GraphicDevice::CreateSwapChain(HWND hWnd, UINT width, UINT height) {
  DXGI_SWAP_CHAIN_DESC1 desc = {};
  desc.Width = width;
  desc.Height = height;
  desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  desc.SampleDesc.Count = 1;
  desc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
  desc.BufferCount = 2;
  desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
  if (FAILED(factory->CreateSwapChainForHwnd(
          commandQueue.Get(), hWnd, &desc, nullptr, nullptr,
          (IDXGISwapChain1 **)swapChain.ReleaseAndGetAddressOf()))) {
    Logger::Log(_T("failed to create swap chain"));
    return false;
  }
  return true;
}
bool GraphicDevice::CreateSwapChainRTV() {
  for (size_t i = 0; i < backBuffers.size(); i++) {
    if (FAILED(swapChain.Get()->GetBuffer(static_cast<UINT>(i),
                                          IID_PPV_ARGS(&backBuffers[i])))) {
      Logger::Log(_T("failed to get buffer"));
      return false;
    }
    rtv.CreateRTV(device.Get(), backBuffers[i].Get());
  }
  Logger::Log(_T("success to get buffer"));
  return true;
}
bool GraphicDevice::CreateFence() {
  if (FAILED(device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE,
                                 IID_PPV_ARGS(&fence)))) {
    Logger::Log(_T("failed to create fence"));
    return false;
  }
  Logger::Log(_T("success to Fence"));
  return true;
}

void GraphicDevice::WaitForCommandQueue() {
  commandQueue->Signal(fence.Get(), ++fenceValue);
  if (fence->GetCompletedValue() != fenceValue) {
    auto event = CreateEvent(nullptr, false, false, nullptr);
    if (!event) {
      Logger::Log(_T("failed to create event"));
      return;
    }
    fence->SetEventOnCompletion(fenceValue, event);
    WaitForSingleObject(event, INFINITE);
    CloseHandle(event);
  }
}

void GraphicDevice::SetBarrier(ID3D12Resource *resource,
                               D3D12_RESOURCE_STATES beforeState,
                               D3D12_RESOURCE_STATES afterState) {
  D3D12_RESOURCE_BARRIER barrier = {};
  barrier.Transition.pResource = resource;
  barrier.Transition.StateBefore = beforeState;
  barrier.Transition.StateAfter = afterState;
  commandList->ResourceBarrier(1, &barrier);
}
void GraphicDevice::ScreenFlip() {
  int backBufferIndex = swapChain->GetCurrentBackBufferIndex();
  SetBarrier(backBuffers[backBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT,
             D3D12_RESOURCE_STATE_RENDER_TARGET);

  D3D12_CPU_DESCRIPTOR_HANDLE handle = rtv.GetHandle(backBufferIndex);
  commandList->OMSetRenderTargets(1, &handle, false, nullptr);

  float clearColor[] = {1.0f, 0.0f, 1.0f, 1.0f};
  commandList->ClearRenderTargetView(handle, clearColor, 0, nullptr);

  SetBarrier(backBuffers[backBufferIndex].Get(),
             D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

  commandList->Close();
  ID3D12CommandList *commandLists[] = {commandList.Get()};
  commandQueue->ExecuteCommandLists(1, commandLists);
  WaitForCommandQueue();
  commandAllocator->Reset();
  commandList->Reset(commandAllocator.Get(), nullptr);
  swapChain->Present(1, 0);
}
void GraphicDevice::EnableDebugLayer() {
  ID3D12Debug *pDebugLayer = nullptr;

  D3D12GetDebugInterface(IID_PPV_ARGS(&pDebugLayer));
  pDebugLayer->EnableDebugLayer();
  pDebugLayer->Release();
}
