#include <array>
#include <iostream>

#include <QString>
#include <QStringLiteral>
#include <QVector>
#include <QTemporaryDir>

#include "Cube.h"
#include "FileName.h"
#include "Histogram.h"
#include "IException.h"
#include "Preference.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "TestUtilities.h"
#include "UserInterface.h"

#include "lrowaccal.h"

#include "gtest/gtest.h"

using namespace Isis;

static const QString APP_XML = FileName("$ISISROOT/bin/xml/lrowaccal.xml").expanded();

TEST(Lrowaccal, FunctionalTestLrowaccalRadianceUnitsLabelExists) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  const QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  const QString testCubeFileName = QStringLiteral("data/lrowaccal/M1388981421CE.tmp.vis.even.reduced.cub");

  QVector<QString> args = {"from=" + testCubeFileName,
                           "to=" + outCubeFileName,
                           "radiometrictype=Radiance",
                           "radiometricfile=Default"};
  UserInterface options(APP_XML, args);

  try {
    lrowaccal(options);
  }
  catch(const IException &e) {
    FAIL() << "Call to lrowaccal failed, unable to calibrate cube: " << e.what() << std::endl;
  }

  Cube outCube(outCubeFileName);

  ASSERT_TRUE(outCube.hasGroup("Radiometry"));
  PvlGroup &radiometry = outCube.group("Radiometry");

  ASSERT_TRUE(radiometry.hasKeyword("RadiometricType"));
  PvlKeyword &radiometricType = radiometry["RadiometricType"];
  ASSERT_EQ(radiometricType.unit().toStdString(), "W/m2/sr/um");
}

TEST(Lrowaccal, FunctionalTestLrowaccalRadianceUnitsLabelNotForIOF) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  const QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  const QString testCubeFileName = QStringLiteral("data/lrowaccal/M1388981421CE.tmp.vis.even.reduced.cub");

  QVector<QString> args = {"from=" + testCubeFileName,
                           "to=" + outCubeFileName,
                           "radiometrictype=IOF",
                           "radiometricfile=Default"};
  UserInterface options(APP_XML, args);

  try {
    lrowaccal(options);
  }
  catch(const IException &e) {
    FAIL() << "Call to lrowaccal failed, unable to calibrate cube: " << e.what() << std::endl;
  }

  Cube outCube(outCubeFileName);

  ASSERT_TRUE(outCube.hasGroup("Radiometry"));
  PvlGroup &radiometry = outCube.group("Radiometry");

  ASSERT_TRUE(radiometry.hasKeyword("RadiometricType"));
  PvlKeyword &radiometricType = radiometry["RadiometricType"];
  ASSERT_NE(radiometricType.unit().toStdString(), "W/m2/sr/um");
}

TEST(Lrowaccal, FunctionalTestLrowaccalBeforeTdr) {
  QTemporaryDir outDir;
  ASSERT_TRUE(outDir.isValid());

  const QString inCubeFileName = QStringLiteral("data/lrowaccal/M1388981421CE.tmp.vis.even.reduced.cub");
  const QString outCubeFileName = outDir.path() + "/M1388981421CE.tmp.vis.even.reduced.cal.cub";

  QVector<QString> args = {
    "from=" + inCubeFileName,
    "to=" + outCubeFileName,
    "dark=true",
    "flatfield=true",
    "radiometric=true",
    "radiometricfile=Default",
    "specialpixels=true",
    "temperature=true",
    "timedependent=false",
    "timedependentfile=Default"};
  UserInterface options{APP_XML, args};

  try {
    lrowaccal(options);
  }
  catch (const IException &exc) {
    FAIL() << "Call to lrowaccal failed, unable to calibrate cube: " << exc.what() << std::endl;
  }

  Cube outCube{outCubeFileName};

  constexpr std::array<double, 5> expectedAverages = {0.0040077573537783, 0.0056263901467155, 0.0059440327288056, 0.0063010299338091, 0.0067947975827685};
  constexpr std::array<double, 5> expectedSums = {5.0497742657607, 7.0892515848615, 7.4894812382951, 7.9392977165994, 8.5614449542883};
  constexpr std::array<int, 5> expectedValidPixels = {1260, 1260, 1260, 1260, 1260};
  constexpr std::array<double, 5> expectedStdDevs = {0.0052071797612098, 0.0075065179425379, 0.0081661724412832, 0.0088797085619543, 0.0094611561024948};

  for (int bandIndex = 0; bandIndex < outCube.bandCount(); bandIndex++) {
    Histogram *outCubeStats = outCube.histogram(bandIndex + 1);
    EXPECT_NEAR(outCubeStats->Average(), expectedAverages.at(bandIndex), 1e-14);
    EXPECT_LT(std::abs((outCubeStats->Sum() - expectedSums.at(bandIndex)) / expectedSums.at(bandIndex)), 1e-14);
    EXPECT_EQ(outCubeStats->ValidPixels(), expectedValidPixels.at(bandIndex));
    EXPECT_NEAR(outCubeStats->StandardDeviation(), expectedStdDevs.at(bandIndex), 1e-14);
  }
}

TEST(Lrowaccal, FunctionalTestLrowaccalAfterTdr) {
  QTemporaryDir outDir;
  ASSERT_TRUE(outDir.isValid());

  const QString inCubeFileName = QStringLiteral("data/lrowaccal/M1388981421CE.tmp.vis.even.reduced.cub");
  const QString outCubeFileName = outDir.path() + "/M1388981421CE.tmp.vis.even.reduced.cal.tdr.cub";

  const QString calibrationDir = (QString) Preference::Preferences().findGroup("DataDirectory")["LRO"] + "/calibration";
  const QString wacTdrCalFile = calibrationDir + "/WAC_TimeDependentCoefficients.0001.pvl";

  QVector<QString> args = {
    "from=" + inCubeFileName,
    "to=" + outCubeFileName,
    "dark=true",
    "flatfield=true",
    "radiometric=true",
    "specialpixels=true",
    "temperature=true",
    "timedependent=true",
    "timedependentfile=" + wacTdrCalFile};
  UserInterface options{APP_XML, args};

  try {
    lrowaccal(options);
  }
  catch (const IException &exc) {
    FAIL() << "Unable to calibrate lrowac image: " << exc.what() << std::endl;
  }

  Cube outCube{outCubeFileName};

  constexpr std::array<double, 5> expectedAverages = {0.0037943872824448, 0.0056235669071665, 0.0060020668851502, 0.0064001848741372, 0.0069595177673183};
  constexpr std::array<double, 5> expectedSums = {4.7809279758804, 7.0856943030298, 7.5626042752893, 8.0642329414129, 8.7689923868211};
  constexpr std::array<int, 5> expectedValidPixels = {1260, 1260, 1260, 1260, 1260};
  constexpr std::array<double, 5> expectedStdDevs = {0.0049299533271533, 0.0075027512929891, 0.0082459023193221, 0.0090194423849291, 0.0096905144251444};

  for (int bandIndex = 0; bandIndex < outCube.bandCount(); bandIndex++) {
    Histogram *outCubeStats = outCube.histogram(bandIndex + 1);
    EXPECT_NEAR(outCubeStats->Average(), expectedAverages.at(bandIndex), 1e-14);
    EXPECT_LT(std::abs((outCubeStats->Sum() - expectedSums.at(bandIndex)) / expectedSums.at(bandIndex)), 1e-14);
    EXPECT_EQ(outCubeStats->ValidPixels(), expectedValidPixels.at(bandIndex));
    EXPECT_NEAR(outCubeStats->StandardDeviation(), expectedStdDevs.at(bandIndex), 1e-14);
  }
}
