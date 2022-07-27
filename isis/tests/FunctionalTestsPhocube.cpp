#include "phocube.h"

#include <QTemporaryFile>
#include <QTextStream>
#include <QStringList>

#include "BandManager.h"
#include "CameraFixtures.h"
#include "Histogram.h"
#include "LineManager.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/phocube.xml").expanded();


TEST_F(DefaultCube, FunctionalTestPhocubeDefault) {
  QString cubeFileName = tempDir.path() + "/phocubeTEMP.cub";
  QVector<QString> args = {"to=" + cubeFileName};
  UserInterface options(APP_XML, args);
  resizeCube(5, 5, 1);
  phocube(testCube, options);

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  ASSERT_EQ(cube.sampleCount(), testCube->sampleCount());
  ASSERT_EQ(cube.lineCount(), testCube->lineCount());
  ASSERT_EQ(cube.bandCount(), 5);

  PvlGroup bandBin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[0], "Phase Angle");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[1], "Emission Angle");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[2], "Incidence Angle");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[3], "Latitude");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[4], "Longitude");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("FilterName"), "CLEAR");
  EXPECT_EQ((int) bandBin.findKeyword("FilterId"), 4);

  for (int i = 0; i < cube.bandCount(); i++) {
    EXPECT_DOUBLE_EQ(bandBin.findKeyword("Center")[i].toDouble(), 1.0);
    EXPECT_DOUBLE_EQ(bandBin.findKeyword("Width")[i].toDouble(), 1.0);
  }

  std::unique_ptr<Histogram> hist (cube.histogram(0));
  EXPECT_NEAR(hist->Average(), 85.320326568603519, .000001);
  EXPECT_NEAR(hist->Sum(), 10665.040821075439, .000001);
  EXPECT_EQ(hist->ValidPixels(), 125);
  EXPECT_NEAR(hist->StandardDeviation(), 90.340311076718081, .000001);

  cube.close();
}


TEST_F(DefaultCube, FunctionalTestPhocubeAllBands) {
//  QString cubeFileName = tempDir.path() + "/phocubeTEMP.cub";
  QString cubeFileName = "phocubeTEMP.cub";
  QVector<QString> args = {"to=" + cubeFileName, "dn=true", "phase=true", "emission=true",
                           "incidence=true", "localemission=true", "localincidence=true",
                           "latitude=true", "longitude=true", "pixelresolution=true",
                           "lineresolution=true", "sampleresolution=true", "detectorresolution=true",
                           "obliquedetectorresolution=true", "northazimuth=true", "sunazimuth=true",
                           "spacecraftazimuth=true", "offnadirangle=true", "subspacecraftgroundazimuth=true",
                           "subsolargroundazimuth=true", "morphology=true", "albedo=true", "radec=true",
                           "bodyfixed=true", "localtime=true"};

  UserInterface options(APP_XML, args);
  resizeCube(5, 5, 1);
  phocube(testCube, options);

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  ASSERT_EQ(cube.sampleCount(), testCube->sampleCount());
  ASSERT_EQ(cube.lineCount(), testCube->lineCount());
  ASSERT_EQ(cube.bandCount(), 27);

  PvlGroup bandBin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[0], "CLEAR");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[1], "Phase Angle");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[2], "Emission Angle");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[3], "Incidence Angle");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[4], "Local Emission Angle");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[5], "Local Incidence Angle");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[6], "Latitude");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[7], "Longitude");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[8], "Pixel Resolution");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[9], "Line Resolution");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[10], "Sample Resolution");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[11], "Detector Resolution");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[12], "Oblique Detector Resolution");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[13], "North Azimuth");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[14], "Sun Azimuth");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[15], "Spacecraft Azimuth");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[16], "OffNadir Angle");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[17], "Sub Spacecraft Ground Azimuth");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[18], "Sub Solar Ground Azimuth");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[19], "Morphology Rank");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[20], "Albedo Rank");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[21], "Right Ascension");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[22], "Declination");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[23], "Body Fixed X");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[24], "Body Fixed Y");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[25], "Body Fixed Z");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[26], "Local Solar Time");

  std::unique_ptr<Histogram> hist (cube.histogram(0));
  EXPECT_NEAR(hist->Average(), -56.952015115651818, .000001);
  EXPECT_NEAR(hist->Sum(), -38442.610203064978, .000001);
  EXPECT_EQ(hist->ValidPixels(), 675);
  EXPECT_NEAR(hist->StandardDeviation(), 667.22433030730758, .000001);

  QVector<double> bandAvg { 13.0,       79.770518,  10.803234,   70.294379,    11.761092,
                            68.010367,  10.087063, 255.646436,   18.841226,    18.841226,
                            18.841226,  18.841226,  19.245272,  333.908975,    91.554202,
                           242.133221,   8.841775, 269.934427,  118.758131,     0.019245,
                             0.069564, 311.691558, -46.862035, -832.758554, -3254.327910,
                           597.579855,   7.769864 };
  QVector<double> bandSum {  325.0,      1994.262954,   270.080856,   1757.359497,    294.027304,
                            1700.259178,  252.176593,  6391.160919,    471.030651,    471.030651,
                             471.030651,  471.030651,   481.131809,   8347.724395,   2288.855072,
                            6053.330535,  221.044387,  6748.360687,   2968.953285,      0.481131,
                               1.739105, 7792.288970, -1171.550888, -20818.963867, -81358.197753,
                           14939.496398,  194.246617 };
  QVector<double> bandValid {25, 25, 25, 25, 25,
                             25, 25, 25, 25, 25,
                             25, 25, 25, 25, 25,
                             25, 25, 25, 25, 25,
                             25, 25, 25, 25, 25,
                             25, 25 };
  QVector<double> bandStd {7.359800,  0.002117,   0.002563, 0.000482, 0.062853,
                           0.061155,  0.000466,   0.000481, 0.000144, 0.000144,
                           0.000144,  0.000144,   0.004518, 0.012455, 0.006764,
                           0.018025,  0.002090,   0.013626, 0.000216, 4.518225e-06,
                           0.000128,  0.003065,   0.002088, 0.027295, 0.007589,
                           0.0278004, 3.211263e-5 };

  for (int i=1; i<=cube.bandCount(); i++) {
    std::unique_ptr<Histogram> hist(cube.histogram(i));
    EXPECT_NEAR(hist->Average(), bandAvg[i-1], 0.000001);
    EXPECT_NEAR(hist->Sum(), bandSum[i-1], 0.000001);
    EXPECT_EQ(hist->ValidPixels(), bandValid[i-1]);
    EXPECT_NEAR(hist->StandardDeviation(), bandStd[i-1], 0.000001);
  }

  cube.close();
}


TEST_F(DefaultCube, FunctionalTestPhocubeSpecialPixels) {
  QString cubeFileName = tempDir.path() + "/phocubeTEMP.cub";
  QVector<QString> args = {"to=" + cubeFileName, "specialpixels=false", "dn=true", "emission=false",
                           "incidence=false", "latitude=false", "longitude=false"};
  UserInterface options(APP_XML, args);

  // Make the testing cube smaller and fill it with special pixels
  resizeCube(5, 5, 1);
  LineManager line(*testCube);
  int pixelValue = 1;
  int lineNum = 0;
  for(line.begin(); !line.end(); line++) {
    for(int i = 0; i < line.size(); i++) {
      if (lineNum == 0) {
        line[i] = (int) pixelValue++;
      }
      else if (lineNum == 1) {
        line[i] = LOW_REPR_SAT1;
      }
      else if (lineNum == 2) {
        line[i] = HIGH_REPR_SAT1;
      }
      else if (lineNum == 3) {
        line[i] = LOW_INSTR_SAT1;
      }
      else if (lineNum == 4) {
        line[i] = HIGH_INSTR_SAT1;
      }
      else {
        line[i] = NULL1;
      }
    }
    testCube->write(line);
    lineNum++;
  }

  phocube(testCube, options);

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  ASSERT_EQ(cube.sampleCount(), testCube->sampleCount());
  ASSERT_EQ(cube.lineCount(), testCube->lineCount());
  ASSERT_EQ(cube.bandCount(), 2);

  PvlGroup bandBin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[0], "CLEAR");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[1], "Phase Angle");

  int band = 1;
  LineManager outLine(cube);
  for (int i = 1; i <= cube.lineCount(); i++) {
    outLine.SetLine(i, band);
    cube.read(outLine);
    for (int j = 0; j < outLine.size(); j++) {
      if (i == 1) { // First line of both bands should match the input cube
        EXPECT_FALSE(IsSpecial(outLine[j]));
      }
      else if (band == 1) { // Rest of the first band are the special pixels from the input cube
        EXPECT_TRUE(IsSpecial(outLine[j]));
      }
      else {  // Rest of the second band should be all NULL
        EXPECT_EQ(outLine[j], NULL8);
      }
    }
    // Check next band
    if (band == 1 && i == cube.lineCount()) {
      i = 1;
      band++;
    }
  }

  std::unique_ptr<Histogram> hist (cube.histogram(0));
  EXPECT_NEAR(hist->Average(), 41.383792877197266, .000001);
  EXPECT_NEAR(hist->Sum(), 413.83792877197266, .0001);
  EXPECT_EQ(hist->ValidPixels(), 10);
  EXPECT_NEAR(hist->StandardDeviation(), 40.473798872303433, .0001);

  cube.close();
}


TEST_F(OffBodyCube, FunctionalTestPhocubeOffBody) {
  QString cubeFileName = tempDir.path() + "/phocubeTEMP.cub";
  QVector<QString> args = {"to=" + cubeFileName, "emission=false", "radec=true",
                           "incidence=false", "latitude=false", "longitude=false"};
  UserInterface options(APP_XML, args);
  phocube(testCube, options);

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  ASSERT_EQ(cube.sampleCount(), testCube->sampleCount());
  ASSERT_EQ(cube.lineCount(), testCube->lineCount());
  ASSERT_EQ(cube.bandCount(), 3);

  PvlGroup bandBin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[0], "Phase Angle");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[1], "Right Ascension");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name")[2], "Declination");

  int band = 1;
  LineManager outLine(cube);
  for (int i = 1; i <= cube.lineCount(); i++) {
    outLine.SetLine(i, band);
    cube.read(outLine);
    for (int j = 0; j < outLine.size(); j++) {
      if (band == 1 && i < 4) { // Phase band will not be null at on-body pixels
        EXPECT_NE(outLine[j], Isis::NULL8);
      }
      else if (band == 1 && i >= 4) { // Phase band will be null at off-body pixels
        EXPECT_EQ(outLine[j], Isis::NULL8);
      }
      else {  // RA and dec bands are not null at all pixels
        EXPECT_NE(outLine[j], Isis::NULL8);
      }
    }
    // Check next band
    if (band == 1 && i == cube.lineCount()) {
      i = 1;
      band++;
    }
  }

  std::unique_ptr<Histogram> hist (cube.histogram(0));
  EXPECT_NEAR(hist->Average(), 130.22882244403544, .000001);
  EXPECT_NEAR(hist->Sum(), 8464.8734588623047, .0001);
  EXPECT_EQ(hist->ValidPixels(), 65);
  EXPECT_NEAR(hist->StandardDeviation(), 167.45747507650518, .0001);

  cube.close();
}


TEST_F(DefaultCube, FunctionalTestPhocubeMosaic) {
  QString cubeFileName = tempDir.path() + "/phocubeTEMP.cub";
  QVector<QString> args = {"to=" + cubeFileName, "source=projection", "dn=true",
                           "latitude=false", "longitude=false"};
  UserInterface options(APP_XML, args);
  resizeCube(5, 5, 1);
  phocube(projTestCube, options);

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  ASSERT_EQ(cube.sampleCount(), testCube->sampleCount());
  ASSERT_EQ(cube.lineCount(), testCube->lineCount());
  ASSERT_EQ(cube.bandCount(), 1);

  PvlGroup bandBin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("FilterName"), "CLEAR");
  EXPECT_EQ((int) bandBin.findKeyword("FilterId"), 4);

  EXPECT_DOUBLE_EQ((double) bandBin.findKeyword("Center"), 1.0);
  EXPECT_DOUBLE_EQ((double) bandBin.findKeyword("Width"), 1.0);

  std::unique_ptr<Histogram> hist (cube.histogram(1));
  EXPECT_NEAR(hist->Average(), 13, .000001);
  EXPECT_NEAR(hist->Sum(), 325, .0001);
  EXPECT_EQ(hist->ValidPixels(), 25);
  EXPECT_NEAR(hist->StandardDeviation(), 7.3598007219398722, .0001);

  cube.close();
}


// Tests that we can process radar data.
TEST_F(MiniRFCube, FunctionalTestPhocubeMiniRF) {
  QString cubeFileName = tempDir.path() + "/phocubeTEMP.cub";
  QVector<QString> args = {"to=" + cubeFileName, "phase=no", "emission=no", "incidence=no",
                           "latitude=no", "longitude=no", "subspacecraftgroundazimuth=yes"};
  UserInterface options(APP_XML, args);
  phocube(testCube, options);

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  ASSERT_EQ(cube.sampleCount(), testCube->sampleCount());
  ASSERT_EQ(cube.lineCount(), testCube->lineCount());
  ASSERT_EQ(cube.bandCount(), 1);

  PvlGroup bandBin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name"), "Sub Spacecraft Ground Azimuth");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("FilterName"), "H RECEIVE INTENSITY");

  LineManager outLine(cube);
  for (int i = 1; i <= cube.lineCount(); i++) {
    outLine.SetLine(i);
    cube.read(outLine);
    for (int j = 0; j < outLine.size(); j++) {
      if (i < 3) {
        EXPECT_TRUE(outLine[j] > 1.0);
      }
      else {
        EXPECT_TRUE(outLine[j] < 1.0);
      }
    }
  }

  std::unique_ptr<Histogram> hist (cube.histogram(1));
  EXPECT_NEAR(hist->Average(), 144.00618486691266, .000001);
  EXPECT_NEAR(hist->Sum(),  3600.1546216728166, .0001);
  EXPECT_EQ(hist->ValidPixels(), 25);
  EXPECT_NEAR(hist->StandardDeviation(), 179.9861282675883, .0001);

  cube.close();
}


TEST_F(DefaultCube, FunctionalTestPhocubeNoBandBin) {
  QString cubeFileName = tempDir.path() + "/phocubeTEMP.cub";
  QVector<QString> args = {"to=" + cubeFileName, "phase=no", "emission=no", "incidence=no",
                           "latitude=no", "longitude=no", "dn=yes"};
  UserInterface options(APP_XML, args);
  resizeCube(5, 5, 1);
  testCube->label()->findObject("IsisCube").deleteGroup("BandBin");
  phocube(testCube, options);


  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  ASSERT_EQ(cube.sampleCount(), testCube->sampleCount());
  ASSERT_EQ(cube.lineCount(), testCube->lineCount());
  ASSERT_EQ(cube.bandCount(), 1);

  PvlGroup bandBin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("Name"), "DN");

  std::unique_ptr<Histogram> hist (cube.histogram(1));
  EXPECT_NEAR(hist->Average(), 13, .000001);
  EXPECT_NEAR(hist->Sum(), 325, .0001);
  EXPECT_EQ(hist->ValidPixels(), 25);
  EXPECT_NEAR(hist->StandardDeviation(), 7.3598007219398722, .0001);

  cube.close();
}


TEST_F(DefaultCube, FunctionalTestPhocubeAllDnBands) {
  QString cubeFileName = tempDir.path() + "/phocubeTEMP.cub";
  QVector<QString> args = {"to=" + cubeFileName, "alldn=true"};

  UserInterface options(APP_XML, args);
  resizeCube(5, 5, 3);
  Pvl *inIsisLabel = testCube->label();
  PvlGroup &inBandBin = inIsisLabel->findGroup("BandBin", Pvl::Traverse);
  inBandBin["FilterName"] = "(B1, B2, B3)";

  phocube(testCube, options);

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  ASSERT_EQ(cube.sampleCount(), testCube->sampleCount());
  ASSERT_EQ(cube.lineCount(), testCube->lineCount());
  ASSERT_EQ(cube.bandCount(), 8);

  PvlGroup bandBin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("FilterName")[0], "B1");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("FilterName")[1], "B2");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandBin.findKeyword("FilterName")[2], "B3");

  // Test band 1
  {
    int band = 1;
    std::unique_ptr<Histogram> inHist (testCube->histogram(band));
    std::unique_ptr<Histogram> outHist (cube.histogram(band));
    EXPECT_NEAR(outHist->Average(), inHist->Average(), .000001);
    EXPECT_NEAR(outHist->Sum(), inHist->Sum(), .000001);
    EXPECT_EQ(outHist->ValidPixels(), inHist->ValidPixels());
    EXPECT_NEAR(outHist->StandardDeviation(), inHist->StandardDeviation(), .000001);
  }

  // Test band 2
  {
    int band = 2;
    std::unique_ptr<Histogram> inHist (testCube->histogram(band));
    std::unique_ptr<Histogram> outHist (cube.histogram(band));
    EXPECT_NEAR(outHist->Average(), inHist->Average(), .000001);
    EXPECT_NEAR(outHist->Sum(), inHist->Sum(), .000001);
    EXPECT_EQ(outHist->ValidPixels(), inHist->ValidPixels());
    EXPECT_NEAR(outHist->StandardDeviation(), inHist->StandardDeviation(), .000001);
  }

  // Test band 3
  {
    int band = 3;
    std::unique_ptr<Histogram> inHist (testCube->histogram(band));
    std::unique_ptr<Histogram> outHist (cube.histogram(band));
    EXPECT_NEAR(outHist->Average(), inHist->Average(), .000001);
    EXPECT_NEAR(outHist->Sum(), inHist->Sum(), .000001);
    EXPECT_EQ(outHist->ValidPixels(), inHist->ValidPixels());
    EXPECT_NEAR(outHist->StandardDeviation(), inHist->StandardDeviation(), .000001);
  }


  cube.close();
}

