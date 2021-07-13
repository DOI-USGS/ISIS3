#include "CSVReader.h"
#include "Fixtures.h"
#include "jitterfit.h"
#include "TestUtilities.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/jitterfit.xml").expanded();
TEST_F(TempTestingFiles, FunctionalTestJitterfitDefault){
  QTemporaryDir prefix;
  QString outputCoeffs= prefix.path()+"/coef.csv";
  QVector<QString> args = {"from=data/jitterfit/simulated_clipper_eis_nac_rolling_shutter_1461_750.cub",
                           "from2=data/jitterfit/simulated_clipper_eis_nac_rolling_shutter_checkline_750.cub",
                           "scale=1.0",
                           "deffile=data/jitterfit/S046mos1400x2250.def",
                           "coefficientto=" + outputCoeffs};

  UserInterface options(APP_XML, args);

  try {
    jitterfit(options);
  }
  catch (IException &e) {
    FAIL() << "Jitterfit failed: " << e.what() << std::endl;
  }

  CSVReader::CSVAxis csvLine;

  CSVReader  csv = CSVReader(outputCoeffs,
                               false, 0, ',', true, true);

  csvLine = csv.getRow(0);
  EXPECT_DOUBLE_EQ(csvLine[0].toDouble(), -2.3393859908317998e-09);
  EXPECT_DOUBLE_EQ(csvLine[1].toDouble(), -7.2806698028439998e-09);
  csvLine = csv.getRow(1);
  EXPECT_DOUBLE_EQ(csvLine[0].toDouble(), 3.9555121090196997e-06);
  EXPECT_DOUBLE_EQ(csvLine[1].toDouble(), 2.6898039797291999e-06);
  csvLine = csv.getRow(2);
  EXPECT_DOUBLE_EQ(csvLine[0].toDouble(), 0.0011490056688787001);
  EXPECT_DOUBLE_EQ(csvLine[1].toDouble(), 0.0036510169712806999);
}
