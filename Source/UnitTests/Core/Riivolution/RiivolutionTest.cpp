#include "Core/Riivolution/RiivolutionXml.h"
#include "Core/Riivolution/RiivolutionPatches.h"

// include order is important
#include <gtest/gtest.h>  // NOLINT

TEST(RiivolutionTest, ConfigParser) {
  rii::RiivolutionXml cfg_nmg(R"(Z:\Users\aaron\Documents\Dolphin Emulator\Games\NMG\riivolution\nmg_usa.xml)");
  rii::RiivolutionXml cfg_newer(R"(Z:\Users\aaron\Documents\Dolphin Emulator\Games\Newer\Newer_Mario_Wii\riivolution\NewerSMBW.xml)");

  rii::RiivolutionPatches::ImportFromXml(cfg_nmg);
  rii::RiivolutionPatches::ImportFromXml(cfg_newer);
}
