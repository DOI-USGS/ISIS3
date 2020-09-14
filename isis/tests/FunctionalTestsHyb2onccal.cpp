// #include <QTemporaryDir>

#include "hyb2onccal.h"

#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Endian.h"
#include "PixelType.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/hyb2onccal.xml").expanded();

TEST_F(Hayabusa2OncTCube, FunctionalTestHyb2onccalIOF) {
  QVector<QString> args = {"to=" + tempDir.path()+"/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  hyb2onccal(testCube, options, &appLog);

  PvlGroup calibrationLog = appLog.findGroup("RadiometricCalibration");
  std::cout<<calibrationLog<<std::endl;

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, calibrationLog["Units"], "I over F");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, calibrationLog["CalibrationUnits"], "IOF");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, calibrationLog["CalibrationFile"], "$hayabusa2/calibration/onc/hyb2oncCalibration0002.trn");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, calibrationLog["FlatFieldFile"], "$hayabusa2/calibration/flatfield/flat_v_norm.cub");
  EXPECT_DOUBLE_EQ(calibrationLog["SensitivityFactor"], 1175.0);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_Bn"][0].toDouble(), 320.66);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_Bn"][1].toDouble(), 0.652);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_Bn"][2].toDouble(), -0.953);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_AECorrection"][0].toDouble(), 0.987);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_AECorrection"][1].toDouble(), 0.00251);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_AETemp"], -1.6);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_CCDTemp"], -28.59);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_ECTTemp"], -11.95);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias"], 310.5920154927201);
  EXPECT_DOUBLE_EQ(calibrationLog["Smear_Tvct"], 0.007373);
  EXPECT_NEAR(calibrationLog["Smear_texp"], 0.0656, .00001);
  EXPECT_DOUBLE_EQ(calibrationLog["RadianceScaleFactor"], 1.0);
  EXPECT_DOUBLE_EQ(calibrationLog["SolarFlux"], 1859.7);
  EXPECT_DOUBLE_EQ(calibrationLog["LinearityCoefficients"][0].toDouble(), 1.0073);
  EXPECT_DOUBLE_EQ(calibrationLog["LinearityCoefficients"][1].toDouble(), -2.9285);
  EXPECT_NEAR(calibrationLog["LinearityCoefficients"][2].toDouble(), -.36434, .000001);
  EXPECT_NEAR(calibrationLog["DarkCurrentCoefficients"][0].toDouble(), 0.1, .0000001);
  EXPECT_DOUBLE_EQ(calibrationLog["DarkCurrentCoefficients"][1].toDouble(), 0.52);
  EXPECT_DOUBLE_EQ(calibrationLog["DarkCurrent"], 0.003961313633742128);
}

TEST_F(Hayabusa2OncTCube, FunctionalTestHyb2onccalRadiance) {
  QVector<QString> args = {"to=" + tempDir.path()+"/output.cub", "units=radiance"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  hyb2onccal(testCube, options, &appLog);

  PvlGroup calibrationLog = appLog.findGroup("RadiometricCalibration");
  std::cout<<calibrationLog<<std::endl;

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, calibrationLog["Units"], "W / (m**2 micrometer sr)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, calibrationLog["CalibrationUnits"], "RADIANCE");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, calibrationLog["CalibrationFile"], "$hayabusa2/calibration/onc/hyb2oncCalibration0002.trn");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, calibrationLog["FlatFieldFile"], "$hayabusa2/calibration/flatfield/flat_v_norm.cub");
  EXPECT_DOUBLE_EQ(calibrationLog["SensitivityFactor"], 1175.0);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_Bn"][0].toDouble(), 320.66);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_Bn"][1].toDouble(), 0.652);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_Bn"][2].toDouble(), -0.953);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_AECorrection"][0].toDouble(), 0.987);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_AECorrection"][1].toDouble(), 0.00251);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_AETemp"], -1.6);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_CCDTemp"], -28.59);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_ECTTemp"], -11.95);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias"], 310.5920154927201);
  EXPECT_DOUBLE_EQ(calibrationLog["Smear_Tvct"], 0.007373);
  EXPECT_NEAR(calibrationLog["Smear_texp"], 0.0656, .00001);
  EXPECT_DOUBLE_EQ(calibrationLog["RadianceScaleFactor"], 1.0);
  EXPECT_DOUBLE_EQ(calibrationLog["SolarFlux"], 1859.7);
  EXPECT_DOUBLE_EQ(calibrationLog["LinearityCoefficients"][0].toDouble(), 1.0073);
  EXPECT_DOUBLE_EQ(calibrationLog["LinearityCoefficients"][1].toDouble(), -2.9285);
  EXPECT_NEAR(calibrationLog["LinearityCoefficients"][2].toDouble(), -.36434, .000001);
  EXPECT_NEAR(calibrationLog["DarkCurrentCoefficients"][0].toDouble(), 0.1, .0000001);
  EXPECT_DOUBLE_EQ(calibrationLog["DarkCurrentCoefficients"][1].toDouble(), 0.52);
  EXPECT_DOUBLE_EQ(calibrationLog["DarkCurrent"], 0.003961313633742128);
}

TEST_F(Hayabusa2OncTCube, FunctionalTestHyb2onccalDN) {
  QVector<QString> args = {"to=" + tempDir.path()+"/output.cub", "units=dn"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  hyb2onccal(testCube, options, &appLog);

  PvlGroup calibrationLog = appLog.findGroup("RadiometricCalibration");
  std::cout<<calibrationLog<<std::endl;

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, calibrationLog["Units"], "DN");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, calibrationLog["CalibrationUnits"], "DN");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, calibrationLog["CalibrationFile"], "$hayabusa2/calibration/onc/hyb2oncCalibration0002.trn");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, calibrationLog["FlatFieldFile"], "$hayabusa2/calibration/flatfield/flat_v_norm.cub");
  EXPECT_DOUBLE_EQ(calibrationLog["SensitivityFactor"], 1175.0);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_Bn"][0].toDouble(), 320.66);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_Bn"][1].toDouble(), 0.652);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_Bn"][2].toDouble(), -0.953);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_AECorrection"][0].toDouble(), 0.987);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_AECorrection"][1].toDouble(), 0.00251);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_AETemp"], -1.6);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_CCDTemp"], -28.59);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias_ECTTemp"], -11.95);
  EXPECT_DOUBLE_EQ(calibrationLog["Bias"], 310.5920154927201);
  EXPECT_DOUBLE_EQ(calibrationLog["Smear_Tvct"], 0.007373);
  EXPECT_NEAR(calibrationLog["Smear_texp"], 0.0656, .00001);
  EXPECT_DOUBLE_EQ(calibrationLog["RadianceScaleFactor"], 1.0);
  EXPECT_DOUBLE_EQ(calibrationLog["SolarFlux"], 1859.7);
  EXPECT_DOUBLE_EQ(calibrationLog["LinearityCoefficients"][0].toDouble(), 1.0073);
  EXPECT_DOUBLE_EQ(calibrationLog["LinearityCoefficients"][1].toDouble(), -2.9285);
  EXPECT_NEAR(calibrationLog["LinearityCoefficients"][2].toDouble(), -.36434, .000001);
  EXPECT_NEAR(calibrationLog["DarkCurrentCoefficients"][0].toDouble(), 0.1, .0000001);
  EXPECT_DOUBLE_EQ(calibrationLog["DarkCurrentCoefficients"][1].toDouble(), 0.52);
  EXPECT_DOUBLE_EQ(calibrationLog["DarkCurrent"], 0.003961313633742128);
}
