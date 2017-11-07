#include "Core/Riivolution/Config/RiivoultionConfig.h"

using namespace Config;

namespace Config::rii {

  const ConfigInfo<bool> RII_ENABLE{ {System::Main, "Riivolution", "RiivoultionEnabled"}, false };
  const ConfigInfo<std::string> RII_SD_ROOT{ {System::Main, "Riivolution", "RiiSDCardRoot"}, "./RiivolutionSD/" };

}
