#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "d3d12.h"
#include <dxgi1_6.h>
#define D3DX12_NO_STATE_OBJECT_HELPERS
#define D3DX12_NO_CHECK_FEATURE_SUPPORT_CLASS
#include "d3dx12/d3dx12.h"

#include <stdio.h>

#include "imgui/imgui.h"

#define ID3D12DeviceN ID3D12Device14
#define ID3D12GraphicsCommandListN ID3D12GraphicsCommandList10
#define ID3D12ResourceN ID3D12Resource2

#define IDXGIFactoryN IDXGIFactory7
#define IDXGIAdapterN IDXGIAdapter4

#define LOG(...) do \
{ \
  fprintf(stderr, __VA_ARGS__); \
  fprintf(stderr, " (%s:%d)\n", __FILE__, __LINE__); \
} while(0)

#define VHR(r) do \
{ \
  if (FAILED(r)) { \
    LOG("[%s()] HRESULT error detected (0x%lX)", __FUNCTION__, r); \
    assert(false); \
    ExitProcess(1); \
  } \
} while(0)
