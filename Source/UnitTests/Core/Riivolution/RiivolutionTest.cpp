#include "Core/RiivolutionConfig.h"

// include order is important
#include <gtest/gtest.h>  // NOLINT

TEST(RiivolutionTest, ConfigParser) {
  rii::RiivolutionConfig cfg_nmg(R"(Z:\Users\aaron\Documents\Dolphin Emulator\Games\NMG\riivolution\nmg_usa.xml)");
  rii::RiivolutionConfig cfg_newer(R"(Z:\Users\aaron\Documents\Dolphin Emulator\Games\Newer\Newer_Mario_Wii\riivolution\NewerSMBW.xml)");
}
