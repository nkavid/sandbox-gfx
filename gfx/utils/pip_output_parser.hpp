#pragma once

#include <re2/re2.h>

#include <cassert>
#include <sstream>
#include <string>

namespace gfx::utils
{
static std::string getMatchingString(const std::string& string, RE2 pattern)
{
  std::string match;
  assert(RE2::PartialMatch(string, pattern, &match));
  return match;
}

static bool parse_pip_output(std::istream& inputStream, std::ostream& outputStream)
{
  std::string string{};
  if (!std::getline(inputStream, string))
  {
    return false;
  }

  RE2 const satisfiedPattern    = "^Requirement already satisfied: ";
  RE2 const requirementsPattern = "from -r requirements.txt";
  if (RE2::PartialMatch(string, satisfiedPattern)
      && RE2::PartialMatch(string, requirementsPattern))
  {
    std::string const required  = getMatchingString(string, ">=([0-9.]*)");
    std::string const installed = getMatchingString(string, "([0-9.]*)[)]$");

    if (required != installed)
    {
      std::string const moduleName = getMatchingString(string, "([[:alnum:]-]*)>=");
      outputStream << moduleName << " required " << required
                   << " does not match installed " << installed << '\n';
    }
    return true;
  }

  RE2 const wouldInstallString{"^Would install"};
  if (RE2::PartialMatch(string, wouldInstallString))
  {
    std::string const wouldInstall = getMatchingString(string,
                                                       "([[:alnum:]-]*-[0-9.]*)");
    outputStream << std::string{"would install: "} << wouldInstall << '\n';
  }

  return true;
}
}
