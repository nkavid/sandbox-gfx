#include "uri.hpp"

#include <cstddef>
#include <cstdint>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size);

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size)
{
  const std::string_view input{reinterpret_cast<const char*>(Data), Size};
  const gfx::URI uri{input};
  return 0;
}
