#pragma once

#include <concepts>
#include <cstddef>

namespace gfx
{
namespace detail
{
class SizeElement
{
  public:
    SizeElement() = delete;

    template <class T>
    explicit constexpr SizeElement(T data)
        : _data{static_cast<size_t>(data)}
    {}

    constexpr bool operator==(const SizeElement&) const = default;

    constexpr bool operator==(const int& other) const
    {
      return _data == static_cast<size_t>(other);
    }

    constexpr size_t operator*(const SizeElement& other) const
    {
      return other._data * _data;
    }

    constexpr size_t operator*(const size_t other) const
    {
      return other * _data;
    }

    constexpr size_t operator/(const std::integral auto&& other) const
    {
      return _data / static_cast<size_t>(other);
    }

    constexpr operator size_t() const
    {
      return _data;
    }

    constexpr operator int() const
    {
      return static_cast<int>(_data);
    }

    constexpr operator unsigned() const
    {
      return static_cast<unsigned>(_data);
    }

    constexpr operator float() const = delete;

  private:
    size_t _data;
};

constexpr bool operator<(size_t lhs, const SizeElement& rhs)
{
  return lhs < static_cast<size_t>(rhs);
}

constexpr size_t operator*(size_t lhs, const SizeElement& rhs)
{
  return lhs * static_cast<size_t>(rhs);
}

} // namespace detail

using Width  = detail::SizeElement;
using Height = detail::SizeElement;

struct Size
{
    Size() = delete;

    template <class T>
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    constexpr Size(T&& pWidth, T&& pHeight)
        : width{pWidth},
          height{pHeight}
    {}

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    constexpr Size(Width&& pWidth, Height&& pHeight)
        : width{pWidth},
          height{pHeight}
    {}

    constexpr bool operator==(const Size&) const = default;

    [[nodiscard]] constexpr size_t size() const
    {
      return width * height;
    }

    Width width;
    Height height;
};
} // namespace gfx
