// SPDX-License-Identifier: CC0-1.0

#include <gtest/gtest.h>

#include <memory>

#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"

#include "PhotoModel.h"
#include "PhotoModelFactory.h"
#include "AtmosModel.h"
#include "AtmosModelFactory.h"
#include "NormModel.h"
#include "NormModelFactory.h"

using namespace Isis;

TEST(AlbedoAtmTest, LegacyTruthMatchesForThreeCases) {
  Preference::Preferences(true);

  // --- Build PVL exactly like legacy unitTest.cpp ---
  PvlGroup algp("Algorithm");
  algp += PvlKeyword("Name", "Lambert");

  PvlObject op("PhotometricModel");
  op.addGroup(algp);

  PvlGroup alga("Algorithm");
  alga += PvlKeyword("Name", "Anisotropic1");
  alga += PvlKeyword("Bha", "0.85");
  alga += PvlKeyword("Tau", "0.28");
  alga += PvlKeyword("Wha", "0.95");
  alga += PvlKeyword("Hga", "0.68");
  alga += PvlKeyword("Tauref", "0.0");
  alga += PvlKeyword("Hnorm", "0.003");

  PvlObject oa("AtmosphericModel");
  oa.addGroup(alga);

  PvlGroup algn("Algorithm");
  algn += PvlKeyword("Name", "AlbedoAtm");
  algn += PvlKeyword("Incref", "0.0");
  algn += PvlKeyword("Thresh", "30.0");

  PvlObject on("NormalizationModel");
  on.addGroup(algn);

  Pvl pvl;
  pvl.addObject(op);
  pvl.addObject(oa);
  pvl.addObject(on);

  // --- Create models via factories (like production code path) ---
  std::unique_ptr<PhotoModel> pm(PhotoModelFactory::Create(pvl));
  ASSERT_NE(pm, nullptr) << "PhotoModelFactory returned null";

  std::unique_ptr<AtmosModel> am(AtmosModelFactory::Create(pvl, *pm));
  ASSERT_NE(am, nullptr) << "AtmosModelFactory returned null";

  std::unique_ptr<NormModel> nm(NormModelFactory::Create(pvl, *pm, *am));
  ASSERT_NE(nm, nullptr) << "NormModelFactory returned null";

  double result = 0.0;
  double mult = 0.0;
  double base = 0.0;

  const double tol = 1.0e-6;

  // Case 1 (from legacy unitTest.cpp / AlbedoAtm.truth)
  nm->CalcNrmAlbedo(
      86.7207248, 51.7031305, 38.9372914,
      51.7031305, 38.9372914,
      0.0800618902,
      result, mult, base);
  EXPECT_NEAR(result, -0.0385293, tol);

  // Case 2
  nm->CalcNrmAlbedo(
      86.7207248, 51.7031305, 38.9372914,
      51.7031305, 38.9372914,
      0.0797334611,
      result, mult, base);
  EXPECT_NEAR(result, -0.0392226, tol);

  // Case 3
  nm->CalcNrmAlbedo(
      86.7187773, 51.7060221, 38.9331391,
      51.7060221, 38.9331391,
      0.0794225037,
      result, mult, base);
  EXPECT_NEAR(result, -0.0398726, tol);
}
