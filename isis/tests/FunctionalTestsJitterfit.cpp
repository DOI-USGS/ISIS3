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
  QString outputRegistrationResults = prefix.path() + "/regResults.csv";

  // Copy input data to tempdir because jitterfit writes to cube labels
  QString dataCube = prefix.path() + "/source.cub";
  std::ifstream dataSrc("data/jitterfit/simulated_clipper_eis_nac_rolling_shutter_1461_750.cub", std::ios::binary);
  std::ofstream dataDest(dataCube.toStdString(), std::ios::binary);
  dataDest << dataSrc.rdbuf();
  dataSrc.close();
  dataDest.close();

  QString checkline = prefix.path() + "/checkline.cub";
  std::ifstream checklineSrc("data/jitterfit/simulated_clipper_eis_nac_rolling_shutter_checkline_750.cub", std::ios::binary);;
  std::ofstream checklineDest(checkline.toStdString(), std::ios::binary);
  checklineDest << checklineSrc.rdbuf();
  checklineSrc.close();
  checklineDest.close();

  QVector<QString> args = {"from=" + dataCube,
                           "from2=" + checkline,
                           "scale=1.0",
                           "deffile=data/jitterfit/S046mos1400x2250.def",
                           "coefficientto=" + outputCoeffs,
                           "residualto=" + outputResiduals,
                           "to=" + outputRegistrationResults,
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

  // line 0, sample 0
  csvLine = csv.getRow(0);
  EXPECT_DOUBLE_EQ(csvLine[0].toDouble(), -3.1003880660954e-09);
  EXPECT_DOUBLE_EQ(csvLine[1].toDouble(), -6.4776611130588001e-09);

  // line 1, sample 1
  csvLine = csv.getRow(1);
  EXPECT_DOUBLE_EQ(csvLine[0].toDouble(), 3.1623419152530002e-06);
  EXPECT_DOUBLE_EQ(csvLine[1].toDouble(), 2.2632154475197e-06);
  // line 2, sample 2
  csvLine = csv.getRow(2);
  EXPECT_DOUBLE_EQ(csvLine[0].toDouble(), 0.0014581710650981999);
  EXPECT_DOUBLE_EQ(csvLine[1].toDouble(), 0.0033661760170435998);

  // Test coefficients that were written to cube
  Cube cube(dataCube);
  Pvl *isisLabel = cube.label();
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);

  EXPECT_DOUBLE_EQ(inst["JitterSampleCoefficients"][0].toDouble(), -6.4776611130587496e-09);
  EXPECT_DOUBLE_EQ(inst["JitterSampleCoefficients"][1].toDouble(), 2.2632154475197199e-06);
  EXPECT_DOUBLE_EQ(inst["JitterSampleCoefficients"][2].toDouble(), 0.0033661760170435998);
  EXPECT_DOUBLE_EQ(inst["JitterLineCoefficients"][0].toDouble(), -3.1003880660953702e-09);
  EXPECT_DOUBLE_EQ(inst["JitterLineCoefficients"][1].toDouble(), 3.1623419152529502e-06);
  EXPECT_DOUBLE_EQ(inst["JitterLineCoefficients"][2].toDouble(), 0.0014581710650981999);

  // Test residuals
  CSVReader::CSVAxis residLine;
  CSVReader residCsv = CSVReader(outputResiduals, false, 0, ',', true, true);

  // first
  residLine = residCsv.getRow(0);
  EXPECT_DOUBLE_EQ(residLine[0].toDouble(), 472.05701420458); // Solved Line
  EXPECT_DOUBLE_EQ(residLine[1].toDouble(), 471.00021366678); // Solved Line
  EXPECT_DOUBLE_EQ(residLine[2].toDouble(), -0.74331356541646); // Registered Line Residual
  EXPECT_DOUBLE_EQ(residLine[3].toDouble(), 376.18491660661); // Solved Sample
  EXPECT_DOUBLE_EQ(residLine[4].toDouble(), 375.00049465222); // Solved Sample
  EXPECT_DOUBLE_EQ(residLine[5].toDouble(), -0.75747819554761); // Sample Residual
  EXPECT_DOUBLE_EQ(residLine[6].toDouble(), -0.52792617775619); // Time Taken

  // middle
  residLine = residCsv.getRow(29);
  EXPECT_DOUBLE_EQ(residLine[0].toDouble(),  1460);
  EXPECT_DOUBLE_EQ(residLine[1].toDouble(), 1460.9998547407);
  EXPECT_DOUBLE_EQ(residLine[2].toDouble(), -0.24038563810288);
  EXPECT_DOUBLE_EQ(residLine[3].toDouble(), 374);
  EXPECT_DOUBLE_EQ(residLine[4].toDouble(), 374.99966574878);
  EXPECT_DOUBLE_EQ(residLine[5].toDouble(), 0.19762410314584);
  EXPECT_DOUBLE_EQ(residLine[6].toDouble(), 0.46284604176785);

  // last
  residLine = residCsv.getRow(59);
  EXPECT_DOUBLE_EQ(residLine[0].toDouble(), 1461);
  EXPECT_DOUBLE_EQ(residLine[1].toDouble(), 1460.9998547407);
  EXPECT_DOUBLE_EQ(residLine[2].toDouble(), -0.81030665929645);
  EXPECT_DOUBLE_EQ(residLine[3].toDouble(), 374);
  EXPECT_DOUBLE_EQ(residLine[4].toDouble(), 374.99966574878);
  EXPECT_DOUBLE_EQ(residLine[5].toDouble(), -0.96593728311905);
  EXPECT_DOUBLE_EQ(residLine[6].toDouble(), 0.46284604176785);


  // Test results of registration
  CSVReader::CSVAxis regLine;
  CSVReader regCsv= CSVReader(outputRegistrationResults, false, 0, ',', true, true);

  // first
  regLine = regCsv.getRow(0);
  EXPECT_DOUBLE_EQ(regLine[0].toDouble(), 471); // Checkline Line
  EXPECT_DOUBLE_EQ(regLine[1].toDouble(), 375); // Checkline Sample
  EXPECT_DOUBLE_EQ(regLine[2].toDouble(), -1); // Checkline Time Taken
  EXPECT_DOUBLE_EQ(regLine[3].toDouble(), 472.05701420458); // Matched Jittered Image Line
  EXPECT_DOUBLE_EQ(regLine[4].toDouble(), 376.18491660661); // Matched Jittered Image Sample
  EXPECT_DOUBLE_EQ(regLine[5].toDouble(), -0.52792617775619); // Matched Jittered image Time Taken
  EXPECT_DOUBLE_EQ(regLine[6].toDouble(), -1.057014204579); // Delta Line
  EXPECT_DOUBLE_EQ(regLine[7].toDouble(), -1.1849166066075); // Delta Sample
  EXPECT_DOUBLE_EQ(regLine[8].toDouble(), 0.98617538549134); // Goodness Of Fit
  EXPECT_DOUBLE_EQ(regLine[9].toDouble(), 1); // Registration Success

  // middle
  regLine = regCsv.getRow(29);
  EXPECT_DOUBLE_EQ(regLine[0].toDouble(), 1461);
  EXPECT_DOUBLE_EQ(regLine[1].toDouble(), 375);
  EXPECT_DOUBLE_EQ(regLine[2].toDouble(), -0.016949152542373);
  EXPECT_DOUBLE_EQ(regLine[3].toDouble(), 1460);
  EXPECT_DOUBLE_EQ(regLine[4].toDouble(), 374);
  EXPECT_DOUBLE_EQ(regLine[5].toDouble(), 0.46284604176785);
  EXPECT_DOUBLE_EQ(regLine[6].toDouble(), 1);
  EXPECT_DOUBLE_EQ(regLine[7].toDouble(), 1);
  EXPECT_DOUBLE_EQ(regLine[8].toDouble(), 0.98756321875905);
  EXPECT_DOUBLE_EQ(regLine[9].toDouble(), 0);

  // Last
  regLine = regCsv.getRow(59);
  EXPECT_DOUBLE_EQ(regLine[0].toDouble(), 1461);
  EXPECT_DOUBLE_EQ(regLine[1].toDouble(), 375);
  EXPECT_DOUBLE_EQ(regLine[2].toDouble(), 1);
  EXPECT_DOUBLE_EQ(regLine[3].toDouble(), 1461);
  EXPECT_DOUBLE_EQ(regLine[4].toDouble(), 374);
  EXPECT_DOUBLE_EQ(regLine[5].toDouble(), 0.46284604176785);
  EXPECT_DOUBLE_EQ(regLine[6].toDouble(), 0);
  EXPECT_DOUBLE_EQ(regLine[7].toDouble(), 1);
  EXPECT_DOUBLE_EQ(regLine[8].toDouble(), 0.9295837101977);
  EXPECT_DOUBLE_EQ(regLine[9].toDouble(), 0);


  // Test statistics of registration
  Pvl stats = Pvl(outputRegistrationStats);
  PvlGroup &autoRegStats = stats.findGroup("AutoRegStatistics");
  EXPECT_EQ((double) autoRegStats.findKeyword("Total"), 60);
  EXPECT_EQ((double) autoRegStats.findKeyword("Successful"), 31);
  EXPECT_EQ((double) autoRegStats.findKeyword("Failure"), 29);

  PvlGroup &success = stats.findGroup("Successes");
  EXPECT_EQ((double) success.findKeyword("SuccessPixel"), 0);
  EXPECT_EQ((double) success.findKeyword("SuccessSubPixel"), 31);

  PvlGroup &patternFailure = stats.findGroup("PatternChipFailures");
  EXPECT_EQ((double) patternFailure.findKeyword("PatternNotEnoughValidData"), 0);
  EXPECT_EQ((double) patternFailure.findKeyword("PatternZScoreNotMet"), 0);

  PvlGroup &fitFailure = stats.findGroup("FitChipFailures");
  EXPECT_EQ((double) fitFailure.findKeyword("FitChipNoData"), 0);
  EXPECT_EQ((double) fitFailure.findKeyword("FitChipToleranceNotMet"), 0);

  PvlGroup &surfaceModelFailures = stats.findGroup("SurfaceModelFailures");
  EXPECT_EQ((double) surfaceModelFailures.findKeyword("SurfaceModelNotEnoughValidData"), 29);
  EXPECT_EQ((double) surfaceModelFailures.findKeyword("SurfaceModelSolutionInvalid"), 0);
  EXPECT_EQ((double) surfaceModelFailures.findKeyword("SurfaceModelDistanceInvalid"), 0);
}
