#include "RiivolutionPatches.h"

#include <set>

#include <locale>
#include <codecvt>
#include <cstdarg>
#include <memory>

std::string format(const char* format, ...)
{
  va_list args;
  va_start(args, format);
#ifndef _MSC_VER
  size_t size = std::snprintf(nullptr, 0, format, args) + 1; // Extra space for '\0'
  std::unique_ptr<char[]> buf(new char[size]);
  std::vsnprintf(buf.get(), size, format, args);
  va_end(args);
  return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
#else
  int size = _vscprintf(format, args);
  std::string result(++size, 0);
  vsnprintf_s((char*)result.data(), size, _TRUNCATE, format, args);
  va_end(args);
  return result;
#endif
}

#if defined(WIN32) || defined(_WIN32) 
#define PATH_SEPARATOR '\\'
#else 
#define PATH_SEPARATOR '/' 
#endif

using namespace rii;

std::vector<std::string> splitpath(
  const std::string& str
  , const std::set<char> delimiters)
{
  std::vector<std::string> result;

  char const* pch = str.c_str();
  char const* start = pch;
  for (; *pch; ++pch)
  {
    if (delimiters.find(*pch) != delimiters.end())
    {
      if (start != pch)
      {
        std::string nstr(start, pch);
        result.push_back(nstr);
      }
      else
      {
        result.push_back("");
      }
      start = pch + 1;
    }
  }
  result.push_back(start);

  return result;
}

bool hasEnding(std::string const &fullString, std::string const &ending) {
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
  }
  else {
    return false;
  }
}


RiivolutionPatches::RiivolutionPatches()
{
}


RiivolutionPatches::~RiivolutionPatches()
{
}

std::map<std::string, GamePatch> patch_map;
std::map<std::string, bool> shiftfiles_map;

void RiivolutionPatches::ImportFromXml(RiivolutionXml& xml)
{
  std::set<char> filepathsep{PATH_SEPARATOR,'.','/'};
  std::string basen = splitpath(std::string(xml.filename), filepathsep).end()[-2];

  std::string name_prefix = format("%s_", basen.c_str());

  bool shiftfiles = xml.shiftfiles;

  for (auto patchp : xml.patches) {
    GamePatch patch = patchp.second;

    std::string id = patchp.first;

    //id = name_prefix + id;
    id = format("%s%s", name_prefix.c_str(), id.c_str());
    patch.id = id.c_str();

    patch.root = (std::string(xml.root_path) + std::string(patch.root)).c_str();

    patch_map.insert_or_assign(id, patch);
    shiftfiles_map.insert_or_assign(id, shiftfiles);
  }

}

std::vector<std::tuple<GamePatch, bool>> RiivolutionPatches::GetPatches()
{
  std::vector<std::tuple<GamePatch, bool>> outp;

  for (auto patchp : patch_map) {
    auto name = patchp.first;
    auto patch = patchp.second;
    bool shiftfiles = shiftfiles_map.at(name);
    outp.push_back(std::tuple<GamePatch, bool>(patch, shiftfiles));
  }

  return outp;
}

std::tuple<GamePatch, bool> RiivolutionPatches::GetByName(std::string name)
{
  auto patch = patch_map.at(name);
  bool shiftfiles = shiftfiles_map.at(name);
  return std::tuple<GamePatch, bool>(patch, shiftfiles);
}

std::vector<std::tuple<GamePatch, bool>> RiivolutionPatches::GetByNameAmbiguous(std::string name)
{
  std::vector<std::tuple<GamePatch, bool>> outp;

  for (auto patchp : patch_map) {
    auto fname = patchp.first;

    if (!hasEnding(fname, name)) continue;

    auto patch = patchp.second;
    bool shiftfiles = shiftfiles_map.at(name);
    outp.push_back(std::tuple<GamePatch, bool>(patch, shiftfiles));
  }

  return outp;
}
