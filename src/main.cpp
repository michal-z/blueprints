import "base.h";
import std;
import gpu;

extern "C" {
  __declspec(dllexport) extern const UINT D3D12SDKVersion = D3D12_SDK_VERSION;
  __declspec(dllexport) extern const char* D3D12SDKPath = ".\\bin\\";
}

static LRESULT CALLBACK window_handle_event(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
  switch (message) {
    case WM_DESTROY: {
      PostQuitMessage(0);
      return 0;
    } break;
    case WM_KEYDOWN: {
      if (wparam == VK_ESCAPE) {
        PostQuitMessage(0);
        return 0;
      }
    } break;
  }
  return DefWindowProcA(hwnd, message, wparam, lparam);
}

static HWND window_create(const char* name, int width, int height)
{
  const WNDCLASSA winclass = {
    .lpfnWndProc = window_handle_event,
    .hInstance = GetModuleHandle(nullptr),
    .hCursor = LoadCursor(nullptr, IDC_ARROW),
    .lpszClassName = name,
  };
  RegisterClass(&winclass);

  const DWORD style = WS_OVERLAPPEDWINDOW;

  RECT rect = { 0, 0, width, height };
  AdjustWindowRectEx(&rect, style, FALSE, 0);

  const HWND window = CreateWindowEx(0, name, name, style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, nullptr, nullptr);

  if (window == NULL) {
    VHR(HRESULT_FROM_WIN32(GetLastError()));
  }

  return window;
}

int main()
{
  CoInitializeEx(nullptr, COINIT_MULTITHREADED);
  SetProcessDPIAware();

  const HWND window = window_create("blueprints", 1920, 1080);

  gpu::init({
    .window = window,
    .num_msaa_samples = 2,
    .ds_target_format = DXGI_FORMAT_R8_UNORM});

  bool running = true;
  while (running) {
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT)
        running = false;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    Sleep(1);
  }

  CoUninitialize();
  return 0;
}
