#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace gfx
{
class URI
{
  public:
    explicit URI(std::string_view uri);

    enum class Schema
    {
      File,
      Unix,
      UDP
    };

    [[nodiscard]] const std::string& string() const;
    [[nodiscard]] const char* c_str() const;

    [[nodiscard]] std::filesystem::path path() const;
    [[nodiscard]] Schema schema() const;

  private:
    std::string _uri;
    Schema _schema;
};
}