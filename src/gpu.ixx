export module gpu;

import "base.h";
import std;
import lib;

namespace gpu
{

constexpr bool enable_debug_layer = true;
constexpr bool enable_debug_layer_gpu = false;

constexpr int max_command_lists = 4;
constexpr int max_buffered_frames = 2;

struct InitArgs
{
  HWND window;
  int num_msaa_samples;
  std::array<float, 4> color_target_clear;
  D3D12_DEPTH_STENCIL_VALUE ds_target_clear;
  DXGI_FORMAT ds_target_format;
};

HWND win_handle;
int win_viewport_w;
int win_viewport_h;

IDXGIFactoryN* dxgi_factory;
IDXGIAdapterN* dxgi_adapter;

export ID3D12DeviceN* device;

export ID3D12CommandQueue* cmd_queue;
std::array<ID3D12CommandAllocator*, max_buffered_frames> cmd_allocators;
std::array<ID3D12GraphicsCommandListN*, max_command_lists> cmd_lists;
int cmd_current_list_index;

ID3D12DebugN* debug;
ID3D12DebugDeviceN* debug_device;
ID3D12DebugCommandQueueN* debug_cmd_queue;
ID3D12DebugCommandListN* debug_cmd_lists[max_command_lists];
ID3D12DebugCommandListN* debug_cmd_current_list;
ID3D12InfoQueueN* debug_info_queue;

export void init(const InitArgs& args)
{
  assert(args.window != nullptr);
  assert(args.num_msaa_samples == 2 || args.num_msaa_samples == 4 || args.num_msaa_samples == 8);

  RECT win_rect = {};
  GetClientRect(args.window, &win_rect);

  win_handle = args.window;
  win_viewport_w = win_rect.right - win_rect.left;
  win_viewport_h = win_rect.bottom - win_rect.top;

  //
  // Factory, adapater, device
  //
  VHR(CreateDXGIFactory2(enable_debug_layer ? DXGI_CREATE_FACTORY_DEBUG : 0, IID_PPV_ARGS(&dxgi_factory)));
  VHR(dxgi_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&dxgi_adapter)));
  {
    DXGI_ADAPTER_DESC3 adapter_desc = {};
    VHR(dxgi_adapter->GetDesc3(&adapter_desc));
    LOG("[gpu] Adapter: %S", adapter_desc.Description);
  }

  if constexpr (enable_debug_layer) {
    if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))) {
      LOG("[gpu] Failed to load D3D12 debug layer. Please rebuild with `enable_debug_layer = false` and try again.");
      ExitProcess(1);
    }
    debug->EnableDebugLayer();
    LOG("[gpu] D3D12 Debug Layer enabled");
    if constexpr (enable_debug_layer_gpu) {
      debug->SetEnableGPUBasedValidation(TRUE);
      LOG("[gpu] D3D12 GPU-Based Validation enabled");
    }
  }

  if (FAILED(D3D12CreateDevice(dxgi_adapter, D3D_FEATURE_LEVEL_11_1, IID_PPV_ARGS(&device)))) {
    MessageBox(win_handle, "Failed to create Direct3D 12 Device. This application requires graphics card with FEATURE LEVEL 11.1 support. Please update your driver and try again.", "DirectX 12 initialization error", MB_OK | MB_ICONERROR);
    ExitProcess(1);
  }
  if constexpr (enable_debug_layer) {
    VHR(device->QueryInterface(IID_PPV_ARGS(&debug_device)));
    VHR(device->QueryInterface(IID_PPV_ARGS(&debug_info_queue)));
    VHR(debug_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE));
  }
  LOG("[gpu] D3D12 device created");

  //
  // Check required features support
  //
  {
    D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
    D3D12_FEATURE_DATA_D3D12_OPTIONS12 options12 = {};
    D3D12_FEATURE_DATA_SHADER_MODEL shader_model = { .HighestShaderModel = D3D_HIGHEST_SHADER_MODEL };

    VHR(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options)));
    VHR(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS12, &options12, sizeof(options12)));
    VHR(device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shader_model, sizeof(shader_model)));

    auto report_and_exit = [](){
      MessageBox(win_handle, "Your graphics card does not support some required features. Please update your graphics driver and try again.", "DirectX 12 initialization error", MB_OK | MB_ICONERROR);
      ExitProcess(1);
    };

    if (options.ResourceBindingTier < D3D12_RESOURCE_BINDING_TIER_3) {
      LOG("[gpu] Resource Binding Tier 3 is NOT SUPPORTED - please update your driver");
      report_and_exit();
    }
    LOG("[gpu] Resource Binding Tier 3 is SUPPORTED");

    if (options12.EnhancedBarriersSupported == FALSE) {
      LOG("[gpu] Enhanced Barriers API is NOT SUPPORTED - please update your driver");
      report_and_exit();
    }
    LOG("[gpu] Enhanced Barriers API is SUPPORTED");

    if (shader_model.HighestShaderModel < D3D_SHADER_MODEL_6_6) {
      LOG("[gpu] Shader Model 6.6 is NOT SUPPORTED - please update your driver");
      report_and_exit();
    }
    LOG("[gpu] Shader Model 6.6 is SUPPORTED");
  }
}

export void deinit()
{
  SAFE_RELEASE(dxgi_adapter);
  SAFE_RELEASE(dxgi_factory);
}

} // namespace gpu
