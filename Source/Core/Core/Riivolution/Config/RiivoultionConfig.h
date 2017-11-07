#pragma once

#include <string>

#include "Common/Config/Config.h"

using namespace Config;

namespace Config::rii {

  extern const ConfigInfo<bool> RII_ENABLE;
  extern const ConfigInfo<std::string> RII_SD_ROOT;

}
