#pragma once

#include <concepts>

namespace gfx
{
class Channel
{
  public:
    explicit constexpr Channel(std::integral auto data)
        : _data{static_cast<float>(data) / _max}
    {
    }

    explicit constexpr Channel(float data)
        : _data{data}
    {
    }

    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr operator float() const
    {
      return _data;
    }

    constexpr bool operator==(const float& other) const
    {
      return (_data <= other + _epsilon) && (_data >= other - _epsilon);
    }

  private:
    float _data;
    constexpr static const float _max{255.0F};
    constexpr static const float _epsilon{1.0F / 256.0F};
};

class Color
{
  public:
    Color() = delete;

    constexpr Color(std::unsigned_integral auto red,
                    std::unsigned_integral auto green,
                    std::unsigned_integral auto blue)
        : _red{red},
          _green{green},
          _blue{blue}
    {
    }

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    constexpr Color(float red, float green, float blue, float alpha = 1.0F)
        : _red{red},
          _green{green},
          _blue{blue},
          _alpha{alpha}
    {
    }

    explicit constexpr Color(std::integral auto rgb)
        // NOLINTBEGIN(readability-magic-numbers)
        : _red{(rgb >> 16) % 256},
          _green{(rgb >> 8) % 256},
          _blue{rgb % 256}
    // NOLINTEND(readability-magic-numbers)
    {
    }

    [[nodiscard]] constexpr Channel red() const
    {
      return _red;
    }

    [[nodiscard]] constexpr Channel green() const
    {
      return _green;
    }

    [[nodiscard]] constexpr Channel blue() const
    {
      return _blue;
    }

    [[nodiscard]] constexpr Channel alpha() const
    {
      return _alpha;
    }

    constexpr bool operator==(const Color& other) const
    {
      const bool redEqual   = _red == static_cast<float>(other._red);
      const bool blueEqual  = _green == static_cast<float>(other._green);
      const bool greenEqual = _blue == static_cast<float>(other._blue);
      const bool alphaEqual = _alpha == static_cast<float>(other._alpha);

      return redEqual && blueEqual && greenEqual && alphaEqual;
    }

  private:
    Channel _red;
    Channel _green;
    Channel _blue;
    Channel _alpha{1.0F};
};

constexpr Color operator+(const Color& dst, const Color& src)
{
  const float red   = src.red() * src.alpha() + dst.red() * (1.0F - src.alpha());
  const float green = src.green() * src.alpha() + dst.green() * (1.0F - src.alpha());
  const float blue  = src.blue() * src.alpha() + dst.blue() * (1.0F - src.alpha());
  const float alpha = src.alpha() + dst.alpha() * (1.0F - src.alpha());

  return Color{red, green, blue, alpha};
}

}
