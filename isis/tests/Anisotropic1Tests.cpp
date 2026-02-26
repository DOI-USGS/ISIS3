/* SPDX-License-Identifier: CC0-1.0 */
#include <gtest/gtest.h>

#include "AtmosModel.h"
#include "AtmosModelFactory.h"
#include "IException.h"
#include "PhotoModel.h"
#include "PhotoModelFactory.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlObject.h"

#include <cmath>
#include <memory>

using namespace Isis;

namespace {

class Anisotropic1Test : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    Preference::Preferences(true);
  }

  static Pvl MakePvl(double tau = 0.28, double bha = 0.85, double hnorm = 0.003,
                     std::optional<double> wha = std::nullopt) {
    PvlGroup photAlg("Algorithm");
    photAlg += PvlKeyword("Name", "Lambert");

    PvlObject photObj("PhotometricModel");
    photObj.addGroup(photAlg);

    PvlGroup atmAlg("Algorithm");
    atmAlg += PvlKeyword("Name", "Anisotropic1");
    atmAlg += PvlKeyword("Tau", toString(tau));
    atmAlg += PvlKeyword("Bha", toString(bha));
    atmAlg += PvlKeyword("Hnorm", toString(hnorm));
    if (wha.has_value()) {
      atmAlg += PvlKeyword("Wha", toString(*wha));
    }

    PvlObject atmObj("AtmosphericModel");
    atmObj.addGroup(atmAlg);

    Pvl pvl;
    pvl.addObject(photObj);
    pvl.addObject(atmObj);
    return pvl;
  }

  static void Calc(AtmosModel &am, double phase, double inc, double emi,
                   double &pstd, double &trans, double &trans0, double &sbar, double &transs) {
    am.CalcAtmEffect(phase, inc, emi, &pstd, &trans, &trans0, &sbar, &transs);
    EXPECT_TRUE(std::isfinite(pstd));
    EXPECT_TRUE(std::isfinite(trans));
    EXPECT_TRUE(std::isfinite(trans0));
    EXPECT_TRUE(std::isfinite(sbar));
    EXPECT_TRUE(std::isfinite(transs));
  }
};

}  // namespace

TEST_F(Anisotropic1Test, MatchesLegacyTruthValues) {
  Pvl pvl = MakePvl();

  std::unique_ptr<PhotoModel> pm(PhotoModelFactory::Create(pvl));
  std::unique_ptr<AtmosModel> am(AtmosModelFactory::Create(pvl, *pm));

  double pstd = 0.0, trans = 0.0, trans0 = 0.0, sbar = 0.0, transs = 0.0;

  // Standard conditions case from legacy unitTest
  am->SetStandardConditions(true);
  Calc(*am, 0.0, 0.0, 0.0, pstd, trans, trans0, sbar, transs);
  EXPECT_NEAR(pstd, 0.0, 1e-12);
  EXPECT_NEAR(trans, 1.0, 1e-12);
  EXPECT_NEAR(trans0, 1.0, 1e-12);
  EXPECT_NEAR(sbar, 0.0, 1e-12);
  am->SetStandardConditions(false);

  // Legacy numeric checks (truth file)
  Calc(*am, 86.7226722, 51.7002388, 38.9414439, pstd, trans, trans0, sbar, transs);
  EXPECT_NEAR(pstd, 0.0983183, 1e-6);
  EXPECT_NEAR(trans, 0.764393, 1e-6);
  EXPECT_NEAR(trans0, 0.444706, 1e-6);
  EXPECT_NEAR(sbar, 0.130632, 1e-6);

  Calc(*am, 180.0, 90.0, 90.0, pstd, trans, trans0, sbar, transs);
  EXPECT_NEAR(pstd, 0.0266486, 1e-6);
  EXPECT_NEAR(trans, 0.159184, 1e-6);
  EXPECT_NEAR(trans0, 5.19719e-07, 1e-11);
  EXPECT_NEAR(sbar, 0.130632, 1e-6);
}

TEST_F(Anisotropic1Test, TauZeroEarlyExitIsStable) {
  Pvl pvl = MakePvl(/*tau=*/0.0);

  std::unique_ptr<PhotoModel> pm(PhotoModelFactory::Create(pvl));
  std::unique_ptr<AtmosModel> am(AtmosModelFactory::Create(pvl, *pm));

  double pstd = -1.0, trans = -1.0, trans0 = -1.0, sbar = -1.0, transs = -1.0;
  Calc(*am, 123.4, 12.3, 45.6, pstd, trans, trans0, sbar, transs);

  EXPECT_NEAR(pstd, 0.0, 1e-12);
  EXPECT_NEAR(trans, 1.0, 1e-12);
  EXPECT_NEAR(trans0, 1.0, 1e-12);
  EXPECT_NEAR(sbar, 0.0, 1e-12);
  EXPECT_NEAR(transs, 1.0, 1e-12);
}

TEST_F(Anisotropic1Test, WhaConservativeCaseThrows) {
  Pvl pvl = MakePvl(/*tau=*/0.28, /*bha=*/0.85, /*hnorm=*/0.003, /*wha=*/1.0);

  std::unique_ptr<PhotoModel> pm(PhotoModelFactory::Create(pvl));
  std::unique_ptr<AtmosModel> am(AtmosModelFactory::Create(pvl, *pm));

  double pstd = 0.0, trans = 0.0, trans0 = 0.0, sbar = 0.0, transs = 0.0;
  EXPECT_THROW(
    am->CalcAtmEffect(10.0, 20.0, 30.0, &pstd, &trans, &trans0, &sbar, &transs),
    IException
  );
}

