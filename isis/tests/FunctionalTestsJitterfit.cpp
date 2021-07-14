#include "CSVReader.h"
#include "Fixtures.h"
#include "jitterfit.h"
#include "TestUtilities.h"

#include "gmock/gmock.h"
#include <fstream>

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/jitterfit.xml").expanded();
TEST_F(TempTestingFiles, FunctionalTestJitterfitDefault){
  QTemporaryDir prefix;
  QString outputCoeffs = prefix.path() + "/coef.csv";
  QString outputResiduals = prefix.path() + "/residuals.csv";
  QString outputRegistrationStats = prefix.path() + "/regStats.pvl";

  // Copy input data to tempdir because jitterfit writes to cube labels
  QString dataCube = prefix.path() + "/source.cub";
  QString checkline = prefix.path() + "/checkline.cub";
  std::ifstream dataSrc("data/jitterfit/simulated_clipper_eis_nac_rolling_shutter_1461_750.cub", std::ios::binary);
  std::ofstream dataDest(dataCube.toStdString(), std::ios::binary);
  std::ifstream checklineSrc("data/jitterfit/simulated_clipper_eis_nac_rolling_shutter_checkline_750.cub", std::ios::binary);;
  std::ofstream checklineDest(checkline.toStdString(), std::ios::binary);
  dataDest << dataSrc.rdbuf();
  checklineDest << checklineSrc.rdbuf();
  dataSrc.close();
  dataDest.close();
  checklineSrc.close();
  checklineDest.close();

  QVector<QString> args = {"from=" + dataCube,
                           "from2=" + checkline,
                           "scale=1.0",
                           "deffile=data/jitterfit/S046mos1400x2250.def",
                           "coefficientto=" + outputCoeffs,
                           "residualto=" + outputResiduals,
                           "to2=" + outputRegistrationStats};

  UserInterface options(APP_XML, args);

  try {
    jitterfit(options);
  }
  catch (IException &e) {
    FAIL() << "Jitterfit failed: " << e.what() << std::endl;
  }

  // Test coefficients that were written to csv
  CSVReader::CSVAxis csvLine;
  CSVReader  csv = CSVReader(outputCoeffs, false, 0, ',', true, true);
  csvLine = csv.getRow(0);

  // line 0, sample 0
  EXPECT_DOUBLE_EQ(csvLine[0].toDouble(), -2.3393859908317998e-09);
  EXPECT_DOUBLE_EQ(csvLine[1].toDouble(), -7.2806698028439998e-09);
  csvLine = csv.getRow(1);
  // line 1, sample 1
  EXPECT_DOUBLE_EQ(csvLine[0].toDouble(), 3.9555121090196997e-06);
  EXPECT_DOUBLE_EQ(csvLine[1].toDouble(), 2.6898039797291999e-06);
  csvLine = csv.getRow(2);
  // line 2, sample 2
  EXPECT_DOUBLE_EQ(csvLine[0].toDouble(), 0.0011490056688787001);
  EXPECT_DOUBLE_EQ(csvLine[1].toDouble(), 0.0036510169712806999);

  // Test coefficients that were written to cube
  Cube cube(dataCube);
  Pvl *isisLabel = cube.label();
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);

  EXPECT_EQ(inst["JitterSampleCoefficients"][0].toStdString(), "-7.28066980284397e-09");
  EXPECT_EQ(inst["JitterSampleCoefficients"][1].toStdString(), "2.68980397972921e-06");
  EXPECT_EQ(inst["JitterSampleCoefficients"][2].toStdString(), "0.0036510169712807");
  EXPECT_EQ(inst["JitterLineCoefficients"][0].toStdString(), "-2.3393859908318e-09");
  EXPECT_EQ(inst["JitterLineCoefficients"][1].toStdString(), "3.95551210901968e-06");
  EXPECT_EQ(inst["JitterLineCoefficients"][2].toStdString(), "0.0011490056688787");

  // Test residuals
  CSVReader::CSVAxis residLine;
  CSVReader residCsv = CSVReader(outputResiduals, false, 0, ',', true, true);

  // first
  residLine = residCsv.getRow(0);
  EXPECT_NEAR(residLine[0].toDouble(), 0, .0000000000000001); // Registered Line
  EXPECT_DOUBLE_EQ(residLine[1].toDouble(), 471.00016795669); // Solved Line
  EXPECT_DOUBLE_EQ(residLine[2].toDouble(), -0.51957457593351); // Registered Line Residual
  EXPECT_NEAR(residLine[3].toDouble(), 0, .0000000000000001); // Registered Sample
  EXPECT_DOUBLE_EQ(residLine[4].toDouble(), 375.00053644333); // Solved Sample
  EXPECT_DOUBLE_EQ(residLine[5].toDouble(), -0.2988303094298); // Sample Residual
  EXPECT_DOUBLE_EQ(residLine[6].toDouble(), -0.52792617775619); // Time Taken

  // middle
  residLine = residCsv.getRow(9);
  EXPECT_DOUBLE_EQ(residLine[0].toDouble(), 967.40156473472996);
  EXPECT_DOUBLE_EQ(residLine[1].toDouble(), 471.00016795669);
  EXPECT_DOUBLE_EQ(residLine[2].toDouble(), 0.13004255779811);
  EXPECT_DOUBLE_EQ(residLine[3].toDouble(), 375.28737801237997);
  EXPECT_DOUBLE_EQ(residLine[4].toDouble(), 375.00053644333);
  EXPECT_DOUBLE_EQ(residLine[5].toDouble(), -0.071082732109573998);
  EXPECT_DOUBLE_EQ(residLine[6].toDouble(), -0.52792617775619);

  // last
  residLine = residCsv.getRow(19);
  EXPECT_DOUBLE_EQ(residLine[0].toDouble(), 966.40657305712);
  EXPECT_DOUBLE_EQ(residLine[1].toDouble(), 966.00000003532);
  EXPECT_DOUBLE_EQ(residLine[2].toDouble(), -0.44508132165397);
  EXPECT_DOUBLE_EQ(residLine[3].toDouble(), 374.29938569993);
  EXPECT_DOUBLE_EQ(residLine[4].toDouble(), 375.00000012271);
  EXPECT_DOUBLE_EQ(residLine[5].toDouble(), -0.2083505883896);
  EXPECT_DOUBLE_EQ(residLine[6].toDouble(), -0.032540067994172);

  // Test stats of registration
  Pvl stats = Pvl(outputRegistrationStats);
  PvlGroup &autoRegStats = stats.findGroup("AutoRegStatistics");
  EXPECT_EQ((double) autoRegStats.findKeyword("Total"), 60);
  EXPECT_EQ((double) autoRegStats.findKeyword("Successful"), 19);
  EXPECT_EQ((double) autoRegStats.findKeyword("Failure"), 41);

  PvlGroup &success = stats.findGroup("Successes");
  EXPECT_EQ((double) success.findKeyword("SuccessPixel"), 0);
  EXPECT_EQ((double) success.findKeyword("SuccessSubPixel"), 19);

  PvlGroup &patternFailure = stats.findGroup("PatternChipFailures");
  EXPECT_EQ((double) patternFailure.findKeyword("PatternNotEnoughValidData"), 40);
  EXPECT_EQ((double) patternFailure.findKeyword("PatternZScoreNotMet"), 0);

  PvlGroup &fitFailure = stats.findGroup("FitChipFailures");
  EXPECT_EQ((double) fitFailure.findKeyword("FitChipNoData"), 0);
  EXPECT_EQ((double) fitFailure.findKeyword("FitChipToleranceNotMet"), 0);

  PvlGroup &surfaceModelFailures = stats.findGroup("SurfaceModelFailures");
  EXPECT_EQ((double) surfaceModelFailures.findKeyword("SurfaceModelNotEnoughValidData"), 1);
  EXPECT_EQ((double) surfaceModelFailures.findKeyword("SurfaceModelSolutionInvalid"), 0);
  EXPECT_EQ((double) surfaceModelFailures.findKeyword("SurfaceModelDistanceInvalid"), 0);
}
