#include <cuda.h>

namespace gfx::compute
{
class Context
{
  public:
    Context();
    ~Context();
    Context(const Context&)            = delete;
    Context& operator=(const Context&) = delete;
    Context(Context&&)                 = delete;
    Context& operator=(Context&&)      = delete;

    [[nodiscard]] CUcontext get() const;

  private:
    CUcontext _cuContext{};
};
}
