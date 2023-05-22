#include "uri.hpp"

#include "utils/logger.hpp"

#include <cstddef>
#include <filesystem>
#include <stdexcept>
#include <string_view>

namespace gfx
{
namespace
{
URI::Schema getSchema(std::string_view uri)
{
  URI::Schema result{};
  if (uri.starts_with("file"))
  {
    result = URI::Schema::File;
  }
  else if (uri.starts_with("unix"))
  {
    result = URI::Schema::Unix;
  }
  else if (uri.starts_with("udp"))
  {
    result = URI::Schema::UDP;
  }
  else
  {
    utils::logger::fatal("invalid schema in uri: ", uri);
  }
  return result;
}
}

URI::URI(std::string_view uri)
    : _uri{uri},
      _schema{getSchema(_uri)}
{}

const char* URI::c_str() const
{
  return _uri.data();
}

std::filesystem::path URI::path() const
{
  if (_schema == Schema::File)
  {
    constexpr size_t schemaPrefixLength = std::string_view("file:").size();
    return _uri.substr(schemaPrefixLength);
  }

  if (_schema == Schema::Unix)
  {
    constexpr size_t schemaPrefixLength = std::string_view("unix:").size();
    return _uri.substr(schemaPrefixLength);
  }

  throw std::runtime_error("gfx::support for path unexpected");
}

URI::Schema URI::schema() const
{
  return _schema;
}
}
