#include "compute_dump.hpp"

#include "compute/detail/check_cuda_call.hpp"
#include "utils/logger.hpp"
#include "vocabulary/size.hpp"

#include <cuda.h>
#include <stb_image_write.h>

#include <cstdint>
#include <string_view>
#include <vector>

namespace gfx::compute::utils
{
void dump_deviceptr(std::string_view filename,
                    CUdeviceptr deviceptr,
                    const gfx::Size& size)
{
  std::vector<uint8_t> buffer{};
  constexpr uint8_t channels{4};

  buffer.resize(size.size() * channels);

  CUCHECK(cuMemcpyDtoH(buffer.data(), deviceptr, buffer.size()));

  stbi_flip_vertically_on_write(static_cast<int>(true));

  if (filename.ends_with(".jpg"))
  {
    static const int quality{100};
    stbi_write_jpg(filename.data(),
                   size.width,
                   size.height,
                   channels,
                   buffer.data(),
                   quality);
  }
  else
  {
    ::gfx::utils::logger::fatal("did not recognize extension for ", filename);
  }
}
}
