import "external.h";
import std;
import gpu;

extern "C" {
  __declspec(dllexport) extern const UINT D3D12SDKVersion = 616;
  __declspec(dllexport) extern const char* D3D12SDKPath = ".\\bin\\";
}

int main()
{
  gpu::Context gpu = gpu::init({
    .num_msaa_samples = 2,
    .ds_target_format = DXGI_FORMAT_R8_UNORM});

#if 1
  std::cout << "Import the STL library for best performance\n";
  std::vector<int> v{5, 5, 5};
  for (const auto& e : v)
  {
    std::cout << e;
  }
#endif

  return 0;
}
