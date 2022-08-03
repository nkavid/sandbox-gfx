#pragma once

#include "vocabulary/size.hpp"
#include "vocabulary/time.hpp"
#include "vocabulary/uri.hpp"

#include <boost/program_options/variables_map.hpp>

#include <filesystem>
#include <optional>

namespace gfx::utils
{
class ArgParser
{
  private:
    std::optional<std::filesystem::path> _outputPath{};
    std::optional<std::filesystem::path> _inputPath{};
    std::optional<gfx::Size> _size{};
    std::optional<gfx::URI> _outputUri{};
    std::optional<gfx::URI> _inputUri{};
    std::optional<gfx::time::sec> _duration{};
    std::optional<gfx::time::fps> _frameRate{};
    bool _verbose{false};

    boost::program_options::variables_map _vm{};

    void _checkForOutputPath();
    void _checkForInputPath();
    void _checkForSize();
    void _checkForOutputUri();
    void _checkForInputUri();
    void _checkForDuration();
    void _checkForFrameRate();
    void _checkForVerbose();

  public:
    ArgParser(int argc, const char* const* argv);

    [[nodiscard]] std::filesystem::path getOutputPath() const;
    [[nodiscard]] std::filesystem::path getInputPath() const;
    [[nodiscard]] gfx::Size getSize() const;
    [[nodiscard]] gfx::URI getOutputUri() const;
    [[nodiscard]] gfx::URI getInputUri() const;
    [[nodiscard]] gfx::time::sec getDuration() const;
    [[nodiscard]] gfx::time::fps getFrameRate() const;
    [[nodiscard]] bool getVerbose() const;
};
}
