/* SPDX-License-Identifier: CC0-1.0 */
#include <gtest/gtest.h>

#include "AtmosModel.h"
#include "AtmosModelFactory.h"
#include "Anisotropic1.h"
#include "IException.h"
#include "NumericalAtmosApprox.h"
#include "PhotoModel.h"
#include "PhotoModelFactory.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"

#include <memory>
#include <vector>
#include <cmath>

using namespace Isis;

namespace {

Pvl MakeBasePvl() {
  Pvl pvl;

  pvl.addObject(PvlObject("PhotometricModel"));
  pvl.findObject("PhotometricModel").addGroup(PvlGroup("Algorithm"));
  pvl.findObject("PhotometricModel")
      .findGroup("Algorithm")
      .addKeyword(PvlKeyword("Name", "Lambert"));

  return pvl;
}

Pvl MakeValidAtmosPvl() {
  Pvl pvl = MakeBasePvl();
  pvl.addObject(PvlObject("AtmosphericModel"));
  pvl.findObject("AtmosphericModel").addGroup(PvlGroup("Algorithm"));
  pvl.findObject("AtmosphericModel")
      .findGroup("Algorithm")
      .addKeyword(PvlKeyword("Name", "Anisotropic1"));
  return pvl;
}

std::unique_ptr<PhotoModel> MakePhotoModel(Pvl &pvl) {
  return std::unique_ptr<PhotoModel>(PhotoModelFactory::Create(pvl));
}

std::unique_ptr<AtmosModel> MakeAtmosModel(Pvl &pvl, PhotoModel &pm) {
  return std::unique_ptr<AtmosModel>(AtmosModelFactory::Create(pvl, pm));
}

void ConfigureCommonAtmosState(AtmosModel &am) {
  am.SetAtmosAtmSwitch(1);
  am.SetAtmosInc(0.0);
  am.SetAtmosPhi(0.0);
  am.SetAtmosHga(0.68);
  am.SetAtmosTau(0.28);
  am.SetAtmosWha(0.98);
}

class AtmosModelTest : public ::testing::Test {
  protected:
    static void SetUpTestSuite() {
      Preference::Preferences(true);
    }
};

TEST_F(AtmosModelTest, FactoryValidation) {
  {
    Pvl pvl = MakeBasePvl();
    auto pm = MakePhotoModel(pvl);
    EXPECT_THROW({
      auto am = MakeAtmosModel(pvl, *pm);
    }, IException);
  }

  {
    Pvl pvl = MakeBasePvl();
    pvl.addObject(PvlObject("AtmosphericModel"));
    auto pm = MakePhotoModel(pvl);
    EXPECT_THROW({
      auto am = MakeAtmosModel(pvl, *pm);
    }, IException);
  }

  {
    Pvl pvl = MakeBasePvl();
    pvl.addObject(PvlObject("AtmosphericModel"));
    pvl.findObject("AtmosphericModel").addGroup(PvlGroup("Algorithm"));
    auto pm = MakePhotoModel(pvl);
    EXPECT_THROW({
      auto am = MakeAtmosModel(pvl, *pm);
    }, IException);
  }

  {
    Pvl pvl = MakeValidAtmosPvl();
    auto pm = MakePhotoModel(pvl);
    auto am = MakeAtmosModel(pvl, *pm);
    ASSERT_NE(am, nullptr);
  }
}

TEST_F(AtmosModelTest, GettersAndStandardConditions) {
  Pvl pvl = MakeValidAtmosPvl();
  auto pm = MakePhotoModel(pvl);
  auto am = MakeAtmosModel(pvl, *pm);

  am->SetAtmosWha(0.98);

  EXPECT_EQ(am->AlgorithmName(), "Unknown");
  EXPECT_NEAR(am->AtmosTau(), 0.28, 1e-10);
  EXPECT_NEAR(am->AtmosWha(), 0.98, 1e-10);
  EXPECT_NEAR(am->AtmosHga(), 0.68, 1e-10);
  EXPECT_FALSE(am->AtmosNulneg());
  EXPECT_EQ(am->AtmosNinc(), 91);

  am->SetStandardConditions(true);

  EXPECT_EQ(am->AlgorithmName(), "Unknown");
  EXPECT_NEAR(am->AtmosTau(), 0.0, 1e-10);
  EXPECT_NEAR(am->AtmosWha(), 0.98, 1e-10);
  EXPECT_NEAR(am->AtmosHga(), 0.68, 1e-10);
  EXPECT_FALSE(am->AtmosNulneg());
  EXPECT_EQ(am->AtmosNinc(), 91);

  am->SetStandardConditions(false);
  am->SetAtmosWha(0.95);
  EXPECT_NEAR(am->AtmosWha(), 0.95, 1e-10);
}

TEST_F(AtmosModelTest, RejectsInvalidSetterValues) {
  Pvl pvl = MakeValidAtmosPvl();
  auto pm = MakePhotoModel(pvl);
  auto am = MakeAtmosModel(pvl, *pm);

  EXPECT_THROW(am->SetAtmosTau(-1.0), IException);
  EXPECT_THROW(am->SetAtmosWha(0.0), IException);
  EXPECT_THROW(am->SetAtmosWha(2.0), IException);
  EXPECT_THROW(am->SetAtmosHga(-1.0), IException);
  EXPECT_THROW(am->SetAtmosHga(1.0), IException);
}

TEST_F(AtmosModelTest, NumericalAtmosApproxFunctionsMatchLegacyTruth) {
  Pvl pvl = MakeValidAtmosPvl();
  auto pm = MakePhotoModel(pvl);
  auto am = MakeAtmosModel(pvl, *pm);

  am->SetAtmosAtmSwitch(1);
  am->SetAtmosInc(0.0);
  am->SetAtmosPhi(0.0);
  am->SetAtmosHga(0.68);
  am->SetAtmosTau(0.28);

  EXPECT_NEAR(NumericalAtmosApprox::InrFunc2Bint(am.get(), 1.0e-6), -5.26033e-07, 1e-10);
  EXPECT_NEAR(NumericalAtmosApprox::OutrFunc2Bint(am.get(), 0.0), 0.195344, 1e-6);

  am->SetAtmosAtmSwitch(2);
  am->SetAtmosInc(3.0);
  am->SetAtmosPhi(78.75);
  am->SetAtmosHga(0.68);
  am->SetAtmosTau(0.28);

  EXPECT_NEAR(NumericalAtmosApprox::InrFunc2Bint(am.get(), 0.75000025), -0.177426, 1e-6);

  am->SetAtmosAtmSwitch(1);
  am->SetAtmosInc(0.0);
  am->SetAtmosPhi(0.0);
  am->SetAtmosHga(0.68);
  am->SetAtmosTau(0.28);

  NumericalAtmosApprox nam;
  EXPECT_NEAR(nam.RombergsMethod(am.get(), NumericalAtmosApprox::OuterFunction, 0.0, 180.0),
              35.1618, 1e-4);
}

TEST_F(AtmosModelTest, RefineExtendedTrapProducesStableValues) {
  Pvl pvl = MakeValidAtmosPvl();
  auto pm = MakePhotoModel(pvl);
  auto am = MakeAtmosModel(pvl, *pm);

  ConfigureCommonAtmosState(*am);

  NumericalAtmosApprox nam;

  double s = 0.0;
  s = nam.RefineExtendedTrap(am.get(), NumericalAtmosApprox::OuterFunction, 0.0, 180.0, s, 1);
  EXPECT_NEAR(s, 35.1618, 1e-4);

  s = 0.0;
  s = nam.RefineExtendedTrap(am.get(), NumericalAtmosApprox::OuterFunction, 0.0, 180.0, s, 2);
  EXPECT_NEAR(s, 17.5809, 1e-4);

  s = 0.0;
  s = nam.RefineExtendedTrap(am.get(), NumericalAtmosApprox::OuterFunction, 0.0, 180.0, s, 9);
  EXPECT_NEAR(s, 17.5809, 1e-4);
}

TEST_F(AtmosModelTest, GenerateAhAndHahgTables) {
  Pvl pvl = MakeValidAtmosPvl();
  auto pm = MakePhotoModel(pvl);
  auto am = MakeAtmosModel(pvl, *pm);

  am->GenerateAhTable();

  EXPECT_EQ(am->AtmosNinc(), 91);
  EXPECT_NEAR(am->AtmosAb(), 0.999898, 1e-6);

  std::vector<double> inc = am->AtmosIncTable();
  std::vector<double> ah = am->AtmosAhTable();

  ASSERT_EQ(inc.size(), 91u);
  ASSERT_EQ(ah.size(), 91u);

  for (int i = 0; i < 91; i++) {
    EXPECT_NEAR(inc[i], static_cast<double>(i), 1e-12);
  }

  EXPECT_NEAR(ah.front(), 1.0, 1e-12);
  EXPECT_NEAR(ah[45], 1.0, 1e-12);
  EXPECT_NEAR(ah[89], 1.0, 1e-12);
  EXPECT_NEAR(ah.back(), 0.0, 1e-12);

  am->GenerateHahgTables();

  EXPECT_NEAR(am->AtmosHahgsb(), -0.085266, 1e-6);

  std::vector<double> hahgt = am->AtmosHahgtTable();
  std::vector<double> hahgt0 = am->AtmosHahgt0Table();

  ASSERT_EQ(hahgt.size(), 91u);
  ASSERT_EQ(hahgt0.size(), 91u);

  EXPECT_NEAR(hahgt[0], 0.0927882, 1e-6);
  EXPECT_NEAR(hahgt[1], 0.0927897, 1e-6);
  EXPECT_NEAR(hahgt[2], 0.0926801, 1e-6);
  EXPECT_NEAR(hahgt[3], 0.0922811, 1e-6);
  EXPECT_NEAR(hahgt[4], 0.0919356, 1e-6);

  for (double v : hahgt) {
    EXPECT_TRUE(std::isfinite(v));
  }
  for (double v : hahgt0) {
    EXPECT_TRUE(std::isfinite(v));
  }
}

TEST_F(AtmosModelTest, SpecialFunctionsMatchKnownValuesAndRejectInvalidInput) {
  EXPECT_NEAR(AtmosModel::En(1, 0.28), 0.957308, 1e-6);
  EXPECT_NEAR(AtmosModel::En(1, 0.733615937), 0.35086, 1e-5);

  EXPECT_NEAR(AtmosModel::Ei(0.234), -0.626785, 1e-6);
  EXPECT_NEAR(AtmosModel::Ei(1.5), 3.30129, 1e-5);
  EXPECT_NEAR(AtmosModel::Ei(2.6), 7.57611, 1e-5);
  EXPECT_NEAR(AtmosModel::Ei(0.01583), -3.55274, 1e-5);

  EXPECT_NEAR(AtmosModel::G11Prime(0.28), 0.79134, 1e-5);
  EXPECT_NEAR(AtmosModel::G11Prime(1.5836), 0.217167, 1e-6);

  EXPECT_THROW(AtmosModel::Ei(0.0), IException);
  EXPECT_THROW(AtmosModel::En(1, 0.0), IException);
  EXPECT_THROW(AtmosModel::En(0, -1.0), IException);
}

}  // namespace