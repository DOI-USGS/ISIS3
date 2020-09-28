#include "hyb2onccal.h"

#include "Fixtures.h"
#include "LineManager.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "SpecialPixel.h"
#include "TestUtilities.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/hyb2onccal.xml").expanded();

TEST_F(Hayabusa2OncTSmallCube, FunctionalTestHyb2onccalIOF) {
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  hyb2onccal(testCube, options, &appLog);

  PvlGroup calibrationLog = appLog.findGroup("RadiometricCalibration");

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

  Cube outputCube;
  try {
    outputCube.open(tempDir.path() + "/output.cub", "r");
  }
  catch (IException &e) {
    // Fail test or throw exception?
    throw IException(IException::User,
                     "Unable to open the file [" + tempDir.path() + "/output.cub" + "] as a cube.",
                     _FILEINFO_);
  }

  LineManager line(outputCube);
  line.SetLine(1);
  outputCube.read(line);
  EXPECT_NEAR(line[0], 0.13324972987174988, .00000000000000001);
  EXPECT_NEAR(line[1], 0.12059289216995239, .00000000000000001);

  line.SetLine(2);
  outputCube.read(line);
  EXPECT_NEAR(line[0], 0.12832331657409668, .00000000000000001);
  EXPECT_NEAR(line[1], 0.11708390712738037, .00000000000000001);
}

TEST_F(Hayabusa2OncTSmallCube, FunctionalTestHyb2onccalSpecialPixel) {
  // Set first pixel to a special pixel
  LineManager line(*testCube);
  line.SetLine(1);
  line[0] = Isis::NULL8;
  line[1] = 100.0;
  testCube->write(line);

  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  hyb2onccal(testCube, options, &appLog);

  Cube outputCube;
  try {
    outputCube.open(tempDir.path() + "/output.cub", "r");
  }
  catch (IException &e) {
    // Fail test or throw exception?
    throw IException(IException::User,
                     "Unable to open the file [" + tempDir.path() + "/output.cub" + "] as a cube.",
                     _FILEINFO_);
  }

  line = LineManager(outputCube);
  line.SetLine(1);
  outputCube.read(line);
  EXPECT_EQ(line[0], Isis::Null);
  EXPECT_NEAR(line[1], 0.11511217057704926, .00000000000000001);
}

TEST_F(Hayabusa2OncTSmallCube, FunctionalTestHyb2onccalSmallDN) {
  // Set first pixel to a DN that is < 100 so that DN * 4 < bias
  LineManager line(*testCube);
  line.SetLine(1);
  line[0] = 50.0;
  line[1] = 100.0;
  testCube->write(line);

  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  hyb2onccal(testCube, options, &appLog);

  Cube outputCube;
  try {
    outputCube.open(tempDir.path() + "/output.cub", "r");
  }
  catch (IException &e) {
    // Fail test or throw exception?
    throw IException(IException::User,
                     "Unable to open the file [" + tempDir.path() + "/output.cub" + "] as a cube.",
                     _FILEINFO_);
  }

  line = LineManager(outputCube);
  line.SetLine(1);
  outputCube.read(line);
  EXPECT_EQ(line[0], Isis::Null);
}

TEST_F(Hayabusa2OncTSmallCube, FunctionalTestHyb2onccalRadiance) {
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub", "units=radiance"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  hyb2onccal(testCube, options, &appLog);

  PvlGroup calibrationLog = appLog.findGroup("RadiometricCalibration");

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

  Cube outputCube;
  try {
    outputCube.open(tempDir.path() + "/output.cub", "r");
  }
  catch (IException &e) {
    // Fail test or throw exception?
    throw IException(IException::User,
                     "Unable to open the file [" + tempDir.path() + "/output.cub" + "] as a cube.",
                     _FILEINFO_);
  }

  LineManager line(outputCube);
  line.SetLine(1);
  outputCube.read(line);
  EXPECT_NEAR(line[0], 5.5233364105224609, .00000000000000001);
  EXPECT_NEAR(line[1], 4.9986977577209473, .00000000000000001);

  line.SetLine(2);
  outputCube.read(line);
  EXPECT_NEAR(line[0], 5.3191318511962891, .00000000000000001);
  EXPECT_NEAR(line[1], 4.8532466888427734, .00000000000000001);
}

TEST_F(Hayabusa2OncTSmallCube, FunctionalTestHyb2onccalDN) {
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub", "units=dn"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  hyb2onccal(testCube, options, &appLog);

  PvlGroup calibrationLog = appLog.findGroup("RadiometricCalibration");

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

  Cube outputCube;
  try {
    outputCube.open(tempDir.path() + "/output.cub", "r");
  }
  catch (IException &e) {
    // Fail test or throw exception?
    throw IException(IException::User,
                     "Unable to open the file [" + tempDir.path() + "/output.cub" + "] as a cube.",
                     _FILEINFO_);
  }

  LineManager line(outputCube);
  line.SetLine(1);
  outputCube.read(line);
  EXPECT_NEAR(line[0], 425.73876953125, .00000000000000001);
  EXPECT_NEAR(line[1], 385.29962158203125, .00000000000000001);

  line.SetLine(2);
  outputCube.read(line);
  EXPECT_NEAR(line[0], 409.9986572265625, .00000000000000001);
  EXPECT_NEAR(line[1], 374.08822631835938, .00000000000000001);
}

TEST_F(Hayabusa2OncTCube, FunctionalTestHyb2onccalNotCropped) {
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  hyb2onccal(testCube, options, &appLog);

  PvlGroup calibrationLog = appLog.findGroup("RadiometricCalibration");

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

  Cube outputCube;
  try {
    outputCube.open(tempDir.path() + "/output.cub", "r");
  }
  catch (IException &e) {
    // Fail test or throw exception?
    throw IException(IException::User,
                     "Unable to open the file [" + tempDir.path() + "/output.cub" + "] as a cube.",
                     _FILEINFO_);
  }

  LineManager line(outputCube);
  line.SetLine(1);
  outputCube.read(line);
  EXPECT_NEAR(line[0], 0.13392314314842224, .00000000000000001);
  EXPECT_NEAR(line[1023], 0.11256968975067139, .00000000000000001);

  line.SetLine(1024);
  outputCube.read(line);
  EXPECT_NEAR(line[0], 0.12710903584957123, .00000000000000001);
  EXPECT_NEAR(line[1023], 0.14761979877948761, .00000000000000001);
}

TEST_F(Hayabusa2OncTSmallCube, FunctionalTestHyb2onccalMultiBand) {
  PvlKeyword &bandKeyword = testCube->label()->findObject("IsisCube").findObject("Core").findGroup("Dimensions").findKeyword("Bands");
  bandKeyword.setValue("2");
  resetCube();
    
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  QString message = "ONC images may only contain one band";
  try {
    hyb2onccal(testCube, options, &appLog);
    FAIL() << "Expected an exception to be thrown";
  }
  catch(Isis::IException &e) {
    EXPECT_PRED_FORMAT2(Isis::AssertIExceptionMessage, e, message);
  }
  catch(...) {
    FAIL() << "Expected error message: \"" << message.toStdString() << "\"";
  }
}

TEST_F(Hayabusa2OncTSmallCube, FunctionalTestHyb2onccalNoFilterName) {
  testCube->label()->findObject("IsisCube").findGroup("BandBin").deleteKeyword("FilterName");
  resetCube();
    
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  QString message = "Unable to read [FilterName] keyword in the BandBin group";
  try {
    hyb2onccal(testCube, options, &appLog);
    FAIL() << "Expected an exception to be thrown";
  }
  catch(Isis::IException &e) {
    EXPECT_PRED_FORMAT2(Isis::AssertIExceptionMessage, e, message);
  }
  catch(...) {
    FAIL() << "Expected error message: \"" << message.toStdString() << "\"";
  }
}

TEST_F(Hayabusa2OncTSmallCube, FunctionalTestHyb2onccalNoInstrumentId) {
  testCube->label()->findObject("IsisCube").findGroup("Instrument").deleteKeyword("InstrumentId");
  resetCube();
    
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  QString message = "Unable to read [InstrumentId] keyword in the Instrument group";
  try {
    hyb2onccal(testCube, options, &appLog);
    FAIL() << "Expected an exception to be thrown";
  }
  catch(Isis::IException &e) {
    EXPECT_PRED_FORMAT2(Isis::AssertIExceptionMessage, e, message);
  }
  catch(...) {
    FAIL() << "Expected error message: \"" << message.toStdString() << "\"";
  }
}

TEST_F(Hayabusa2OncTSmallCube, FunctionalTestHyb2onccalInvalidId) {
  PvlKeyword &idKeyword = testCube->label()->findObject("IsisCube").findGroup("Instrument")
                                                                   .findKeyword("InstrumentId");
  idKeyword.setValue("invalid");
  resetCube();
    
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  QString message = "Unidentified instrument key in the "
                    "InstrumentId key of the Instrument Pvl group.";
  try {
    hyb2onccal(testCube, options, &appLog);
    FAIL() << "Expected an exception to be thrown";
  }
  catch(Isis::IException &e) {
    EXPECT_PRED_FORMAT2(Isis::AssertIExceptionMessage, e, message);
  }
  catch(...) {
    FAIL() << "Expected error message: \"" << message.toStdString() << "\"";
  }
}

TEST_F(Hayabusa2OncTSmallCube, FunctionalTestHyb2onccalNegativeBitDepth) {
  PvlKeyword &depthKeyword = testCube->label()->findObject("IsisCube").findGroup("Instrument")
                                                                      .findKeyword("BitDepth");
  depthKeyword.setValue("-1");
  resetCube();
    
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  hyb2onccal(testCube, options, &appLog);

  Cube outputCube;
  try {
    outputCube.open(tempDir.path() + "/output.cub", "r");
  }
  catch (IException &e) {
    // Fail test or throw exception?
    throw IException(IException::User,
                     "Unable to open the file [" + tempDir.path() + "/output.cub" + "] as a cube.",
                     _FILEINFO_);
  }

  LineManager line(outputCube);
  line.SetLine(1);
  outputCube.read(line);
  // If pixel ^ (12 - bit_depth) < 300, we set pixel to NULL
  ASSERT_EQ(line[0], Isis::Null);
}

TEST_F(Hayabusa2OncTSmallCube, FunctionalTestHyb2onccalNoBitDepth) {
  testCube->label()->findObject("IsisCube").findGroup("Instrument").deleteKeyword("BitDepth");
  resetCube();
    
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  hyb2onccal(testCube, options, &appLog);

  Cube outputCube;
  try {
    outputCube.open(tempDir.path() + "/output.cub", "r");
  }
  catch (IException &e) {
    // Fail test or throw exception?
    throw IException(IException::User,
                     "Unable to open the file [" + tempDir.path() + "/output.cub" + "] as a cube.",
                     _FILEINFO_);
  }

  LineManager line(outputCube);
  line.SetLine(1);
  outputCube.read(line);
  // If pixel ^ (12 - bit_depth) < 300, we set pixel to NULL
  ASSERT_EQ(line[0], Isis::Null);
}

TEST_F(Hayabusa2OncTSmallCube, FunctionalTestHyb2onccalNoExposure) {
  testCube->label()->findObject("IsisCube").findGroup("Instrument").deleteKeyword("ExposureDuration");
  resetCube();
    
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  QString message = "Unable to read [ExposureDuration] keyword in the Instrument group";
  try {
    hyb2onccal(testCube, options, &appLog);
    FAIL() << "Expected an exception to be thrown";
  }
  catch(Isis::IException &e) {
    EXPECT_PRED_FORMAT2(Isis::AssertIExceptionMessage, e, message);
  }
  catch(...) {
    FAIL() << "Expected error message: \"" << message.toStdString() << "\"";
  }
}

TEST_F(Hayabusa2OncTSmallCube, FunctionalTestHyb2onccalNoAETemp) {
  testCube->label()->findObject("IsisCube").findGroup("Instrument").deleteKeyword("ONCAETemperature");
  resetCube();
    
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  QString message = "Unable to read [ONCAETemperature] keyword in the Instrument group";
  try {
    hyb2onccal(testCube, options, &appLog);
    FAIL() << "Expected an exception to be thrown";
  }
  catch(Isis::IException &e) {
    EXPECT_PRED_FORMAT2(Isis::AssertIExceptionMessage, e, message);
  }
  catch(...) {
    FAIL() << "Expected error message: \"" << message.toStdString() << "\"";
  }
}

TEST_F(Hayabusa2OncTSmallCube, FunctionalTestHyb2onccalNoCCDTemp) {
  testCube->label()->findObject("IsisCube").findGroup("Instrument").deleteKeyword("ONCTCCDTemperature");
  resetCube();
    
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  QString message = "Unable to read [ONCTCCDTemperature] keyword in the Instrument group";
  try {
    hyb2onccal(testCube, options, &appLog);
    FAIL() << "Expected an exception to be thrown";
  }
  catch(Isis::IException &e) {
    EXPECT_PRED_FORMAT2(Isis::AssertIExceptionMessage, e, message);
  }
  catch(...) {
    FAIL() << "Expected error message: \"" << message.toStdString() << "\"";
  }
}

TEST_F(Hayabusa2OncTSmallCube, FunctionalTestHyb2onccalNoECTemp) {
  testCube->label()->findObject("IsisCube").findGroup("Instrument").deleteKeyword("ONCTElectricCircuitTemperature");
  resetCube();
    
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  QString message = "Unable to read [ONCTElectricCircuitTemperature] keyword in the Instrument group";
  try {
    hyb2onccal(testCube, options, &appLog);
    FAIL() << "Expected an exception to be thrown";
  }
  catch(Isis::IException &e) {
    EXPECT_PRED_FORMAT2(Isis::AssertIExceptionMessage, e, message);
  }
  catch(...) {
    FAIL() << "Expected error message: \"" << message.toStdString() << "\"";
  }
}

TEST_F(Hayabusa2OncTSmallCube, FunctionalTestHyb2onccalNoClockCount) {
  testCube->label()->findObject("IsisCube").findGroup("Instrument").deleteKeyword("SpacecraftClockStartCount");
  resetCube();
    
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  QString message = "Unable to read [SpacecraftClockStartCount] keyword in the Instrument group";
  try {
    hyb2onccal(testCube, options, &appLog);
    FAIL() << "Expected an exception to be thrown";
  }
  catch(Isis::IException &e) {
    EXPECT_PRED_FORMAT2(Isis::AssertIExceptionMessage, e, message);
  }
  catch(...) {
    FAIL() << "Expected error message: \"" << message.toStdString() << "\"";
  }
}

TEST_F(Hayabusa2OncTSmallCube, FunctionalTestHyb2onccalNoSolarDistance) {
  testCube->label()->findObject("IsisCube").findGroup("Instrument").deleteKeyword("SolarDistance");
  resetCube();
    
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  QString message = "Unable to read [SolarDistance] keyword in the Instrument group";
  try {
    hyb2onccal(testCube, options, &appLog);
    FAIL() << "Expected an exception to be thrown";
  }
  catch(Isis::IException &e) {
    EXPECT_PRED_FORMAT2(Isis::AssertIExceptionMessage, e, message);
  }
  catch(...) {
    FAIL() << "Expected error message: \"" << message.toStdString() << "\"";
  }
}

TEST_F(Hayabusa2OncTSmallCube, FunctionalTestHyb2onccalNoSmearCorrection) {
  testCube->label()->findObject("IsisCube").findGroup("Instrument").deleteKeyword("SmearCorrection");
  resetCube();
    
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  QString message = "Unable to read [SmearCorrection] keyword in the Instrument group";
  try {
    hyb2onccal(testCube, options, &appLog);
    FAIL() << "Expected an exception to be thrown";
  }
  catch(Isis::IException &e) {
    EXPECT_PRED_FORMAT2(Isis::AssertIExceptionMessage, e, message);
  }
  catch(...) {
    FAIL() << "Expected error message: \"" << message.toStdString() << "\"";
  }
}
