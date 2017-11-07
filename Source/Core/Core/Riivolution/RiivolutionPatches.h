#pragma once
#include "Core/Riivolution/RiivolutionXml.h"

#include <tuple>

namespace rii {

  class RiivolutionPatches
  {
    RiivolutionPatches();
    ~RiivolutionPatches();
  public:
    static void ImportFromXml(RiivolutionXml& xml);

    // Tuples in form <patch,shiftfiles>

    static std::vector<std::tuple<GamePatch,bool>> GetPatches();
    static std::tuple<GamePatch, bool> GetByName(std::string name);
    static std::vector<std::tuple<GamePatch, bool>> GetByNameAmbiguous(std::string name);
  };

}
