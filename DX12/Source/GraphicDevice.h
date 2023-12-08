#pragma once
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DCompiler.lib")
#include <array>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

class RtvHeap {
public:
  RtvHeap();
  virtual ~RtvHeap();
  bool Create(ID3D12Device *device, int numDescriptors);
  int CreateRTV(ID3D12Device *device, ID3D12Resource *resource);
  D3D12_CPU_DESCRIPTOR_HANDLE GetHandle(int index) const;

private:
  int nextRegistNumber = 0;
  int rtvDescriptorSize = 0;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap = nullptr;
};
class GraphicDevice {
public:
  GraphicDevice();
  virtual ~GraphicDevice();
  bool Create(HWND hWnd, UINT width, UINT height);
  void ScreenFlip();
  ID3D12GraphicsCommandList6 *GetCommandList() const {
    return commandList.Get();
  }
  ID3D12Device8 *GetDevice() const { return device.Get(); }

private:
  UINT64 fenceValue = 0;
  RtvHeap rtv;
  Microsoft::WRL::ComPtr<ID3D12Device8> device = nullptr;
  Microsoft::WRL::ComPtr<IDXGIFactory6> factory = nullptr;
  Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> commandList = nullptr;
  Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
  Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
  std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> backBuffers;
  Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
  bool CreateDevice();
  bool CreateFactory();
  bool CreateCommandList();
  bool CreateSwapChain(HWND hWnd, UINT width, UINT height);
  bool CreateSwapChainRTV();
  bool CreateFence();
  void WaitForCommandQueue();
  void SetBarrier(ID3D12Resource *resource, D3D12_RESOURCE_STATES beforeState,
                  D3D12_RESOURCE_STATES afterState);

  void EnableDebugLayer();
};
