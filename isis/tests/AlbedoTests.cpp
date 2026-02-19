/* SPDX-License-Identifier: CC0-1.0 */
#include <gtest/gtest.h>

#include "PhotoModel.h"
#include "PhotoModelFactory.h"
#include "NormModel.h"
#include "NormModelFactory.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlObject.h"
#include "Preference.h"
#include "SpecialPixel.h"
#include "IException.h"

#include <memory>

using namespace Isis;

namespace {

class AlbedoTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    Preference::Preferences(true);
  }

  static Pvl MakeLambertAlbedoPvl() {
    PvlGroup algp("Algorithm");
    algp += PvlKeyword("Name", "Lambert");
    PvlObject op("PhotometricModel");
    op.addGroup(algp);

    PvlGroup algn("Algorithm");
    algn += PvlKeyword("Name", "Albedo");
    PvlObject on("NormalizationModel");
    on.addGroup(algn);

    Pvl pvl;
    pvl.addObject(op);
    pvl.addObject(on);
    return pvl;
  }

  static std::unique_ptr<PhotoModel> MakePhotoModel(Pvl &pvl) {
    return std::unique_ptr<PhotoModel>(PhotoModelFactory::Create(pvl));
  }

  static std::unique_ptr<NormModel> MakeNormModel(Pvl &pvl, PhotoModel &pm) {
    return std::unique_ptr<NormModel>(NormModelFactory::Create(pvl, pm));
  }
};

}  // namespace

TEST_F(AlbedoTest, LambertNadirIsIdentityAndMultIsOne) {
  Pvl pvl = MakeLambertAlbedoPvl();
  auto pm = MakePhotoModel(pvl);
  auto nm = MakeNormModel(pvl, *pm);

  double result = 0.0, mult = 0.0, base = 0.0;
  const double dn = 0.0800618902;

  nm->CalcNrmAlbedo(0.0, 0.0, 0.0, 0.0, 0.0, dn, result, mult, base);

  // Legacy truth prints 0.0800619; enforce a tight but safe tolerance.
  EXPECT_NEAR(result, dn, 1e-12);
  EXPECT_NEAR(mult, 1.0, 1e-12);
  EXPECT_NEAR(base, 0.0, 1e-12);
}

TEST_F(AlbedoTest, LambertOffNadirMatchesLegacyTruth) {
  Pvl pvl = MakeLambertAlbedoPvl();
  auto pm = MakePhotoModel(pvl);
  auto nm = MakeNormModel(pvl, *pm);

  double result = 0.0, mult = 0.0, base = 0.0;

  const double phase = 86.7207248;
  const double inc   = 51.7031305;
  const double ema   = 38.9372914;
  const double dn    = 0.0797334611;

  nm->CalcNrmAlbedo(phase, inc, ema, inc, ema, dn, result, mult, base);

  // From Albedo.truth
  EXPECT_NEAR(result, 0.128657, 1e-6);
  EXPECT_NEAR(base, 0.0, 1e-12);

  // Mult should be the ratio between reference and current photometric response,
  // so it should be > 1 here given result > dn.
  EXPECT_GT(mult, 1.0);
}

TEST_F(AlbedoTest, ExtremeAnglesReturnNull8AndZeroMultBase) {
  Pvl pvl = MakeLambertAlbedoPvl();
  auto pm = MakePhotoModel(pvl);
  auto nm = MakeNormModel(pvl, *pm);

  double result = 0.0, mult = 0.0, base = 0.0;

  // From Albedo.truth (result prints as -1.79769e+308 == NULL8)
  nm->CalcNrmAlbedo(180.0, 90.0, 90.0, 90.0, 90.0, 0.0794225037, result, mult, base);

  EXPECT_EQ(result, NULL8);
  EXPECT_NEAR(mult, 0.0, 1e-12);
  EXPECT_NEAR(base, 0.0, 1e-12);
}

TEST_F(AlbedoTest, InvalidIncrefThrows) {
  // incref must be >=0 and <90 per Albedo::SetNormIncref
  Pvl pvl;

  PvlGroup algp("Algorithm");
  algp += PvlKeyword("Name", "Lambert");
  PvlObject op("PhotometricModel");
  op.addGroup(algp);

  PvlGroup algn("Algorithm");
  algn += PvlKeyword("Name", "Albedo");
  algn += PvlKeyword("Incref", "90.0");  // invalid
  PvlObject on("NormalizationModel");
  on.addGroup(algn);

  pvl.addObject(op);
  pvl.addObject(on);

  auto pm = MakePhotoModel(pvl);

  EXPECT_THROW(
    {
      auto nm = MakeNormModel(pvl, *pm);
      (void)nm;
    },
    IException
  );
}