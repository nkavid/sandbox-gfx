#include "application.hpp"
#include "utils/logger.hpp"

#include <cstdlib>
#include <exception>

int main()
{
  try
  {
    gfx::vulkan::Application application{};
    application.initVulkan();
    application.mainLoop();
    application.terminate();
  }
  catch (const std::exception& e)
  {
    gfx::utils::logger::error(e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
