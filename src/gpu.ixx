export module gpu;

import "external.h";
import std;

namespace gpu
{

constexpr bool enable_debug_layer = true;

constexpr UINT max_command_lists = 4;
constexpr UINT max_buffered_frames = 2;

export struct Context
{
  HWND win_handle;
  LONG win_viewport_w;
  LONG win_viewport_h;

  IDXGIFactory7* dxgi_factory;
  IDXGIAdapter4* dxgi_adapter;

  ID3D12DeviceN* device;

  ID3D12CommandQueue* cmd_queue;
  ID3D12CommandAllocator* cmd_allocators[max_buffered_frames];
  ID3D12GraphicsCommandListN* cmd_lists[max_command_lists];
  ID3D12GraphicsCommandListN* cmd_current_list;
  UINT cmd_current_list_index;
};

struct InitArgs
{
  HWND window;
  UINT num_msaa_samples;
  std::array<float, 4> color_target_clear;
  D3D12_DEPTH_STENCIL_VALUE ds_target_clear;
  DXGI_FORMAT ds_target_format;
};

export Context init(const InitArgs& args)
{
  assert(args.window != nullptr);
  assert(args.num_msaa_samples == 2 || args.num_msaa_samples == 4 || args.num_msaa_samples == 8);

  RECT win_rect = {};
  GetClientRect(args.window, &win_rect);

  //
  // Factory, adapater, device
  //
  IDXGIFactoryN* dxgi_factory = nullptr;
  vhr(CreateDXGIFactory2(enable_debug_layer ? DXGI_CREATE_FACTORY_DEBUG : 0, IID_PPV_ARGS(&dxgi_factory)));

  IDXGIAdapterN* dxgi_adapter = nullptr;
  vhr(dxgi_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&dxgi_adapter)));

  return {
    .win_handle = args.window,
    .win_viewport_w = win_rect.right,
    .win_viewport_h = win_rect.bottom,
    .dxgi_factory = dxgi_factory,
    .dxgi_adapter = dxgi_adapter,
  };
}

} // namespace gpu
