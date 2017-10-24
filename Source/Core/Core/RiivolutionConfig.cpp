#include "Core/RiivolutionConfig.h"
#include <iostream>
#include <pugixml.hpp>

namespace rii {

RiivolutionConfig::RiivolutionConfig(wchar_t* filename)
{
  pugi::xml_document doc;

  pugi::xml_parse_result result = doc.load_file(filename);
}

RiivolutionConfig::~RiivolutionConfig()
{
}

}
