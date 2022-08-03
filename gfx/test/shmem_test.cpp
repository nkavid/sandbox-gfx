#include "reader.hpp"
#include "writer.hpp"

#include <catch2/catch_test_macros.hpp>
#include <unistd.h>

#include <array>

SCENARIO("Basic read write operation", "[gfx][shmem][reader][writer]")
{
  GIVEN("Reader and writer pair and message buffers")
  {
    constexpr size_t bufferSize{256};

    gfx::shmem::Writer writer{"shmem_test", bufferSize};
    gfx::shmem::Reader reader{"shmem_test", bufferSize};

    std::array<char, bufferSize> readBuffer{};
    std::array<char, bufferSize> writeBuffer{};

    // NOLINTBEGIN(readability-magic-numbers)
    writeBuffer[0] = '0';
    writeBuffer[1] = '1';
    writeBuffer[2] = '2';
    writeBuffer[3] = '3';
    // NOLINTEND(readability-magic-numbers)

    WHEN("No messsage has been written")
    {
      REQUIRE(reader.read(readBuffer.data()) == false);
    }

    WHEN("a message is written")
    {
      writer.write(writeBuffer.data());
      THEN("Received the message")
      {
        REQUIRE(reader.read(readBuffer.data()) == true);
        REQUIRE(readBuffer == writeBuffer);
      }
    }

    WHEN("writing several messages")
    {
      writer.write(writeBuffer.data());
      writer.write(writeBuffer.data());

      // NOLINTBEGIN(readability-magic-numbers)
      writeBuffer[3] = '9';
      // NOLINTEND(readability-magic-numbers)

      writer.write(writeBuffer.data());
      THEN("Read the latest message")
      {
        REQUIRE(reader.read(readBuffer.data()) == true);
        REQUIRE(readBuffer == writeBuffer);

        REQUIRE(reader.read(readBuffer.data()) == false);
      }
    }
  }
}
