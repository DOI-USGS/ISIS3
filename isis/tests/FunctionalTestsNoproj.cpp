#include "noproj.h"

#include <QTextStream>
#include "CameraFixtures.h"
#include "Histogram.h"
#include "LineManager.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "skypt.h"
#include "getsn.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/noproj.xml").expanded();
static QString APP_XML1 = FileName("$ISISROOT/bin/xml/skypt.xml").expanded();

TEST_F(DefaultCube, FunctionalTestNoprojDefault) {
  QString cubeFileName = tempDir.path() + "/output.cub";
  QVector<QString> args = {"to=" + cubeFileName};
  UserInterface options(APP_XML, args);

  // Empty match cube
  noproj(testCube, NULL, options);

  Cube oCube(cubeFileName);
  Pvl *isisLabel = oCube.label();
  PvlGroup instGroup = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("SpacecraftName"), "IdealSpacecraft");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentId"), "IdealCamera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("TargetName"), "MARS");
  EXPECT_EQ((int) instGroup.findKeyword("SampleDetectors"), 1250);
  EXPECT_EQ((int) instGroup.findKeyword("LineDetectors"), 1150);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentType"), "FRAMING");
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("EphemerisTime"), -709401200.26114);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("StartTime"), "1977-07-09T20:05:51");
  EXPECT_EQ((int) instGroup.findKeyword("FocalPlaneXDependency"), 1);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransX"), 1.0);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransY"), 1.0);

  FileName matchedCubeName(instGroup.findKeyword("matchedCube"));
  FileName defaultCubeName(testCube->fileName());
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, matchedCubeName.name(), defaultCubeName.name());

  PvlGroup origInst = isisLabel->findGroup("OriginalInstrument", Pvl::Traverse);
  PvlGroup testCubeInst = testCube->label()->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupKeywordsEqual, origInst, testCubeInst); //REFACTOR

  std::unique_ptr<Histogram> hist (oCube.histogram(1));
  EXPECT_NEAR(hist->Average(), 127.4782522807407, 4.6e-06);
  EXPECT_NEAR(hist->Sum(), 166492334, 6);
  EXPECT_EQ(hist->ValidPixels(), 1306045);
  EXPECT_NEAR(hist->StandardDeviation(), 68.405508539707895, .0001);
}

TEST_F(DefaultCube, DISABLED_FunctionalTestNoprojExpand) {
  QString cubeFileName = tempDir.path() + "/output.cub";
  QVector<QString> args = {"to=" + cubeFileName, "sampexp=10", "lineexp=5"};
  UserInterface options(APP_XML, args);

  noproj(testCube, NULL, options);

  Cube oCube(cubeFileName);
  Pvl *isisLabel = oCube.label();
  PvlGroup instGroup = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("SpacecraftName"), "IdealSpacecraft");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentId"), "IdealCamera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("TargetName"), "MARS");
  EXPECT_EQ((int) instGroup.findKeyword("SampleDetectors"), 1375);
  EXPECT_EQ((int) instGroup.findKeyword("LineDetectors"), 1208);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentType"), "FRAMING");
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("EphemerisTime"), -709401200.26114);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("StartTime"), "1977-07-09T20:05:51");
  EXPECT_EQ((int) instGroup.findKeyword("FocalPlaneXDependency"), 1);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransX"), 1.0);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransY"), 1.0);

  FileName matchedCubeName(instGroup.findKeyword("matchedCube"));
  FileName defaultCubeName(testCube->fileName());
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, matchedCubeName.name(), defaultCubeName.name());

  PvlGroup origInst = isisLabel->findGroup("OriginalInstrument", Pvl::Traverse);
  PvlGroup testCubeInst = testCube->label()->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupKeywordsEqual, origInst, testCubeInst); //REFACTOR

  std::unique_ptr<Histogram> hist (oCube.histogram(1));
  EXPECT_NEAR(hist->Average(), 127.50009071999523, .000001);
  EXPECT_NEAR(hist->Sum(), 166542786, 1);
  EXPECT_EQ(hist->ValidPixels(), 1306217);
  EXPECT_NEAR(hist->StandardDeviation(), 68.416277416274923, .0001);
}

// Test source parameter
// Since match cube = input cube, frommatch and frominput will
// give the same output. Default test already tests frommatch.
TEST_F(DefaultCube, FunctionalTestNoprojFromInput) {
  QString cubeFileName = tempDir.path() + "/output.cub";
  QVector<QString> args = {"to=" + cubeFileName, "source=frominput"};
  UserInterface options(APP_XML, args);

  noproj(testCube, NULL, options);

  Cube oCube(cubeFileName);
  Pvl *isisLabel = oCube.label();
  PvlGroup instGroup = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("SpacecraftName"), "IdealSpacecraft");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentId"), "IdealCamera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("TargetName"), "MARS");
  EXPECT_EQ((int) instGroup.findKeyword("SampleDetectors"), 1250);
  EXPECT_EQ((int) instGroup.findKeyword("LineDetectors"), 1150);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentType"), "FRAMING");
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("EphemerisTime"), -709401200.26114);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("StartTime"), "1977-07-09T20:05:51");
  EXPECT_EQ((int) instGroup.findKeyword("FocalPlaneXDependency"), 1);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransX"), 1.0);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransY"), 1.0);

  FileName matchedCubeName(instGroup.findKeyword("matchedCube"));
  FileName defaultCubeName(testCube->fileName());
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, matchedCubeName.name(), defaultCubeName.name());

  PvlGroup origInst = isisLabel->findGroup("OriginalInstrument", Pvl::Traverse);
  PvlGroup testCubeInst = testCube->label()->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupKeywordsEqual, origInst, testCubeInst); //REFACTOR

  std::unique_ptr<Histogram> hist (oCube.histogram(1));
  EXPECT_NEAR(hist->Average(), 127.4782522807407, .00001);
  EXPECT_NEAR(hist->Sum(), 166492334, 13); // A large number needs a large tol on Mac
  EXPECT_EQ(hist->ValidPixels(), 1306045);
  EXPECT_NEAR(hist->StandardDeviation(), 68.405508539707895, .0001);
}

TEST_F(DefaultCube, FunctionalTestNoprojFromUser) {
  QString cubeFileName = tempDir.path() + "/output.cub";
  QVector<QString> args = {"to=" + cubeFileName, "source=fromuser", "sum=2"};
  UserInterface options(APP_XML, args);

  noproj(testCube, NULL, options);

  Cube oCube(cubeFileName);
  Pvl *isisLabel = oCube.label();
  PvlGroup instGroup = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("SpacecraftName"), "IdealSpacecraft");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentId"), "IdealCamera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("TargetName"), "MARS");
  EXPECT_EQ((int) instGroup.findKeyword("SampleDetectors"), 625);
  EXPECT_EQ((int) instGroup.findKeyword("LineDetectors"), 1150);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentType"), "FRAMING");
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("EphemerisTime"), -709401200.26114);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("StartTime"), "1977-07-09T20:05:51");
  EXPECT_EQ((int) instGroup.findKeyword("FocalPlaneXDependency"), 1);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransX"), 1.0);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransY"), 1.0);

  FileName matchedCubeName(instGroup.findKeyword("matchedCube"));
  FileName defaultCubeName(testCube->fileName());
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, matchedCubeName.name(), defaultCubeName.name());

  PvlGroup origInst = isisLabel->findGroup("OriginalInstrument", Pvl::Traverse);
  PvlGroup testCubeInst = testCube->label()->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupKeywordsEqual, origInst, testCubeInst); //REFACTOR

  std::unique_ptr<Histogram> hist (oCube.histogram(1));
  EXPECT_NEAR(hist->Average(), 127.46759565451696, 9.2e-06);
  EXPECT_NEAR(hist->Sum(), 41629897, 3);
  EXPECT_EQ(hist->ValidPixels(), 326592);
  EXPECT_NEAR(hist->StandardDeviation(), 68.444806666131768, .0001);
}

TEST_F(DefaultCube, FunctionalTestNoprojSpecs) {
  std::ofstream specsStream;
  QString specsFileName = tempDir.path() + "/specs.pvl";
  specsStream.open(specsFileName.toStdString());
  specsStream << R"(
    Object = IdealInstrumentsSpecifications
      UserName     = ssides/sgstapleton
      Created      = 2006-12-14T10:10:49
      LastModified = 2019-06-27
      # 2019-11-05 Modified by ladoramkershner: Added Mariner 10

      # Group name and values will change once stabilized
      Group = "Clipper EIS 2025/EIS-NAC-RS"
        DetectorSamples = 4000
        DetectorLines = 2000
      End_Group

      Group = "Clipper EIS 2025/EIS-WAC-FC"
        DetectorSamples = 4000
        DetectorLines = 2000
      End_Group

      # Max offset from undistorted to distorted < 1
      Group = "HAYABUSA-2/ONC-T"
        DetectorSamples = 1025
        DetectorLines   = 1025
      End_Group

      # TBD (W1 images currently have very small data coverage)
      Group = "HAYABUSA-2/ONC-W1"
        DetectorSamples = 1025
        DetectorLines   = 1025
      End_Group

      # Max offset from undistorted to distorted: (2.15, 5.67)
      Group = "HAYABUSA-2/ONC-W2"
        DetectorSamples = 1027
        DetectorLines   = 1030
      End_Group

      Group = "MARS GLOBAL SURVEYOR/MOC-WA"
        DetectorSamples = 6000
      End_Group

      Group = "MARS RECONNAISSANCE ORBITER/CTX"
        DetectorSamples = 4991
        TransY = -0.39031635
        ItransS = 55.759479
      End_Group
      Group = "MARS GLOBAL SURVEYOR/MOC-WA"
        DetectorSamples = 6000
      End_Group

      Group = "MARS RECONNAISSANCE ORBITER/CTX"
        DetectorSamples = 4991
        TransY = -0.39031635
        ItransS = 55.759479
      End_Group

      Group = "MARS GLOBAL SURVEYOR/MOC-WA"
        DetectorSamples = 6000
      End_Group

      Group = "MARS RECONNAISSANCE ORBITER/CTX"
        DetectorSamples = 4991
        TransY = -0.39031635
        ItransS = 55.759479
      End_Group

      Group = "MARS RECONNAISSANCE ORBITER/HIRISE"
        DetectorSamples = 20000
    #    Use the average of red ccd's 4 & 5 for the offsets
        TransX = -92.9979
        ItransL = 7749.8250
      End_Group

      Group = "Messenger/MDIS-NAC"
        DetectorSamples = 1034
        DetectorLines = 1034
      End_Group

      Group = "Messenger/MDIS-WAC"
        DetectorSamples = 1034
        DetectorLines = 1034
      End_Group

      Group = "NEW HORIZONS/LEISA"
        DetectorSamples = 256
      End_Group

      Group = "TRACE GAS ORBITER/CaSSIS"
        DetectorSamples = 2048
        DetectorLines = 2048
      End_Group

      Group = "VIKING_ORBITER_1/VISUAL_IMAGING_SUBSYSTEM_CAMERA_B"
        DetectorSamples = 2000
        DetectorLines = 1000
      End_Group

      Group = "VIKING_ORBITER_1/VISUAL_IMAGING_SUBSYSTEM_CAMERA_A"
        DetectorSamples = 1250
        DetectorLines = 1150
      End_Group

      Group = "VIKING_ORBITER_2/VISUAL_IMAGING_SUBSYSTEM_CAMERA_B"
        DetectorSamples = 1250
        DetectorLines = 1150
      End_Group

      Group = "VIKING_ORBITER_2/VISUAL_IMAGING_SUBSYSTEM_CAMERA_A"
        DetectorSamples = 1250
        DetectorLines = 1150
      End_Group

      Group = "VOYAGER_1/NARROW_ANGLE_CAMERA"
        DetectorSamples = 1000
        DetectorLines = 1000
      End_Group

      Group = "VOYAGER_1/WIDE_ANGLE_CAMERA"
        DetectorSamples = 1000
        DetectorLines = 1000
      End_Group

      Group = "VOYAGER_2/NARROW_ANGLE_CAMERA"
        DetectorSamples = 1000
        DetectorLines = 1000
      End_Group

      Group = "VOYAGER_2/WIDE_ANGLE_CAMERA"
        DetectorSamples = 1000
        DetectorLines = 1000
      End_Group

      Group = "MARINER_10/M10_VIDICON_A"
        DetectorSamples = 832
        DetectorLines = 700
      End_Group

      Group = "MARINER_10/M10_VIDICON_B"
        DetectorSamples = 832
        DetectorLines = 700
      End_Group
    End_Object
    End
  )";
  specsStream.close();

  QString cubeFileName = tempDir.path() + "/output.cub";
  QVector<QString> args = {"to=" + cubeFileName, "specs=" + specsFileName};
  UserInterface options(APP_XML, args);

  noproj(testCube, NULL, options);

  Cube oCube(cubeFileName);
  Pvl *isisLabel = oCube.label();
  PvlGroup instGroup = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("SpacecraftName"), "IdealSpacecraft");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentId"), "IdealCamera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("TargetName"), "MARS");
  EXPECT_EQ((int) instGroup.findKeyword("SampleDetectors"), 2000);
  EXPECT_EQ((int) instGroup.findKeyword("LineDetectors"), 1000);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentType"), "FRAMING");
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("EphemerisTime"), -709401200.26114);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("StartTime"), "1977-07-09T20:05:51");
  EXPECT_EQ((int) instGroup.findKeyword("FocalPlaneXDependency"), 1);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransX"), 1.0);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransY"), 1.0);

  FileName matchedCubeName(instGroup.findKeyword("matchedCube"));
  FileName defaultCubeName(testCube->fileName());
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, matchedCubeName.name(), defaultCubeName.name());

  PvlGroup origInst = isisLabel->findGroup("OriginalInstrument", Pvl::Traverse);
  PvlGroup testCubeInst = testCube->label()->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupKeywordsEqual, origInst, testCubeInst); //REFACTOR

  std::unique_ptr<Histogram> hist (oCube.histogram(1));
  EXPECT_NEAR(hist->Average(), 127.53053767760592, 3.7e-06);
  EXPECT_NEAR(hist->Sum(), 153645476, 4.5);
  EXPECT_EQ(hist->ValidPixels(), 1204774);
  EXPECT_NEAR(hist->StandardDeviation(), 68.420632943519294, .0001);
}

TEST_F(LineScannerCube, FunctionalTestNoprojLineScanner) {
  // Create KAGUYA/TC2 specs entry so that we can use the LineScannerCube fixture.
  std::ofstream specsStream;
  QString specsFileName = tempDir.path() + "/specs.pvl";
  specsStream.open(specsFileName.toStdString());
  specsStream << R"(
    Object = IdealInstrumentsSpecifications
      Group = "KAGUYA/TC2"
        DetectorSamples = 2000
        DetectorLines = 1000
      End_Group
    End_Object
    End
  )";
  specsStream.close();

  QString cubeFileName = tempDir.path() + "/output.cub";
  QVector<QString> args = {"to=" + cubeFileName, "specs=" + specsFileName};
  UserInterface options(APP_XML, args);

  noproj(testCube, NULL, options);

  Cube oCube(cubeFileName);
  Pvl *isisLabel = oCube.label();
  PvlGroup instGroup = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("SpacecraftName"), "IdealSpacecraft");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentId"), "IdealCamera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("TargetName"), "MOON");
  EXPECT_EQ((int) instGroup.findKeyword("SampleDetectors"), 2000);
  EXPECT_EQ((int) instGroup.findKeyword("LineDetectors"), 1);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentType"), "LINESCAN");
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("EphemerisTime"), 266722396.06431001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("StartTime"), "2008-06-14T13:32:10.933207");
  EXPECT_EQ((int) instGroup.findKeyword("FocalPlaneXDependency"), 1);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransX"), -1.0);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransY"), -1.0);

  FileName matchedCubeName(instGroup.findKeyword("matchedCube"));
  FileName defaultCubeName(testCube->fileName());
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, matchedCubeName.name(), defaultCubeName.name());

  PvlGroup origInst = isisLabel->findGroup("OriginalInstrument", Pvl::Traverse);
  PvlGroup testCubeInst = testCube->label()->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupKeywordsEqual, origInst, testCubeInst); //REFACTOR

  std::unique_ptr<Histogram> hist (oCube.histogram(1));
  EXPECT_NEAR(hist->Average(), 26.259903800874739, .000001);
  EXPECT_NEAR(hist->Sum(), 78070.6940000006, .0001);
  EXPECT_EQ(hist->ValidPixels(), 2973);
  EXPECT_NEAR(hist->StandardDeviation(), 11.938337629048096, .0001);
}

/**
   * Test Noproj parameters OFFBODY=TRUE and OFFBODYTRIM=FALSE
   * Call Skypt to check that pixels in noprojed image 
   * have the expected values (Null or valid). 
   *
   * Input ...
   *   1) From cube, Level 1 cube, spiceinited, cropped MapCam cube.
   *   2) Match cube, Level 1 cube, spiceinited, cropped MapCam cube.
   * Output ...
   *   1) Noproj'ed cube.
   *   2) Pvl log file from Skypt.
   *
  */
TEST_F(DefaultCube, FunctionalTestNoprojOffBodyTrueOffBodyTrimFalse) { 

  QString iCubeName = "data/osirisRexImages/20190509T180552S020_crop.cub";
  QString mCubeName = "data/osirisRexImages/20190509T174620S424_crop.cub";
  QString noprojCubeName = tempDir.path() + "/output.cub";
  QVector<QString> args = {"to=" + noprojCubeName, "offbody=TRUE", "offbodytrim=FALSE"};

  Cube iCube;
  Cube mCube;
  iCube.open(iCubeName);
  mCube.open(mCubeName);
  
  // Run noproj with match cube specified.
  UserInterface options(APP_XML, args); 
  noproj(&iCube, &mCube, options);

  // Check results of noproj cube.
  Cube noprojCube(noprojCubeName);
  Pvl *isisLabel = noprojCube.label();
  PvlGroup instGroup = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("SpacecraftName"), "IdealSpacecraft");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentId"), "IdealCamera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("TargetName"), "Bennu");
  EXPECT_EQ((int) instGroup.findKeyword("SampleDetectors"), 1050);
  EXPECT_EQ((int) instGroup.findKeyword("LineDetectors"), 1050);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentType"), "FRAMING");
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("EphemerisTime"), 610696049.67661);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("StartTime"), "2019-05-09T17:46:20.424");
  EXPECT_EQ((int) instGroup.findKeyword("FocalPlaneXDependency"), 2);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransX"), 1.0);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransY"), 1.0);

  FileName matchedCubeName(instGroup.findKeyword("MatchedCube"));
  FileName inputCubeName(mCube.fileName());
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, matchedCubeName.name(), inputCubeName.name());
  
  
  // Call Skypt on an off-body sky pixel and check for valid pixel value.
  QVector<QString> argsSkypt1 = {"from="+noprojCube.fileName(),
                                 "format=pvl",
                                 "type=image", 
                                 "sample=20.0", 
                                 "line=30.0"};                
  UserInterface uiSkypt1(APP_XML1, argsSkypt1);
  Pvl appLogSkypt1;
  skypt(&noprojCube, uiSkypt1, &appLogSkypt1);
  PvlGroup skyPoint1 = appLogSkypt1.findGroup("SkyPoint");

  EXPECT_DOUBLE_EQ( (double) skyPoint1.findKeyword("Sample"), 20.0);
  EXPECT_DOUBLE_EQ( (double) skyPoint1.findKeyword("Line"), 30.0);
  EXPECT_NEAR( (double) skyPoint1.findKeyword("RightAscension"), 181.5703153396, 1e-8);
  EXPECT_NEAR( (double) skyPoint1.findKeyword("Declination"), -1.712838512527, 1e-8);
  EXPECT_NEAR( (double) skyPoint1.findKeyword("EphemerisTime"), 610696049.67661, 1e-8);
  EXPECT_NEAR( (double) skyPoint1.findKeyword("PixelValue"), 0.00034996954, 1e-8);
  EXPECT_NEAR( (double) skyPoint1.findKeyword("CelestialNorthClockAngle"), 239.06687506777, 1e-8);

  // Call Skypt on an off-body pixel and check for valid pixel value.
  QVector<QString> argsSkypt2 = {"from="+noprojCube.fileName(),
                                 "format=pvl",
                                 "type=image", 
                                 "sample=100.0", 
                                 "line=200.0"};                
  UserInterface uiSkypt2(APP_XML1, argsSkypt2);
  Pvl appLogSkypt2;
  skypt(&noprojCube, uiSkypt2, &appLogSkypt2);
  PvlGroup skyPoint2 = appLogSkypt2.findGroup("SkyPoint");
 
  // On-body pixel is expected to be valid.
  EXPECT_DOUBLE_EQ( (double) skyPoint2.findKeyword("Sample"), 100.0);
  EXPECT_DOUBLE_EQ( (double) skyPoint2.findKeyword("Line"), 200.0);
  EXPECT_NEAR( (double) skyPoint2.findKeyword("RightAscension"), 180.84505524757, 1e-8);
  EXPECT_NEAR( (double) skyPoint2.findKeyword("Declination"), -1.6394524986874, 1e-8);
  EXPECT_NEAR( (double) skyPoint2.findKeyword("EphemerisTime"), 610696049.67661, 1e-8);
  EXPECT_NEAR( (double) skyPoint2.findKeyword("PixelValue"), 0.027573904, 1e-8);
  EXPECT_NEAR( (double) skyPoint2.findKeyword("CelestialNorthClockAngle"), 239.03639925925, 1e-8);

 // Call Skypt on an on-body pixel and check for valid pixel value.
  QVector<QString> argsSkypt3 = {"from="+noprojCube.fileName(),
                                 "format=pvl",
                                 "type=image", 
                                 "sample=500.0", 
                                 "line=500.0"};                
  UserInterface uiSkypt3(APP_XML1, argsSkypt3); 
  Pvl appLogSkypt3;
  skypt(&noprojCube, uiSkypt3, &appLogSkypt3);
  PvlGroup skyPoint3 = appLogSkypt3.findGroup("SkyPoint");
 
  // On-body pixel is expected to be valid.
  EXPECT_DOUBLE_EQ( (double) skyPoint3.findKeyword("Sample"), 500.0);
  EXPECT_DOUBLE_EQ( (double) skyPoint3.findKeyword("Line"), 500.0);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("RightAscension"), 179.04549552762, 1e-8);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("Declination"), -2.370616350112, 1e-8);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("EphemerisTime"), 610696049.67661, 1e-8);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("PixelValue"), 0.030219724, 1e-8);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("CelestialNorthClockAngle"), 238.96087237795, 1e-8);

}

/*
   * Test Noproj parameters OFFBODY=TRUE and OFFBODYTRIM=TRUE
   * Call Skypt to check that pixels in noprojed image 
   * have the expected values (Null or valid). 
   *
   * Input ...
   *   1) From cube, Level 1 cube, spiceinited, cropped MapCam cube.
   *   2) Match cube, Level 1 cube, spiceinited, cropped MapCam cube.
   * Output ...
   *   1) Noproj'ed cube.
   *   2) Pvl log file from Skypt.
   *
*/
TEST_F(DefaultCube, FunctionalTestNoprojOffBodyTrueOffBodyTrimTrue) { 

  QString iCubeName = "data/osirisRexImages/20190509T180552S020_crop.cub";
  QString mCubeName = "data/osirisRexImages/20190509T174620S424_crop.cub";
  QString noprojCubeName = tempDir.path() + "/output.cub";
  QVector<QString> args = {"to=" + noprojCubeName, "offbody=TRUE", "offbodytrim=TRUE"};

  Cube iCube;
  Cube mCube;
  iCube.open(iCubeName);
  mCube.open(mCubeName);
  
  // Run noproj with match cube specified.
  UserInterface options(APP_XML, args); 
  noproj(&iCube, &mCube, options);

  // Check results of noproj cube.
  Cube noprojCube(noprojCubeName);
  Pvl *isisLabel = noprojCube.label();
  PvlGroup instGroup = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("SpacecraftName"), "IdealSpacecraft");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentId"), "IdealCamera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("TargetName"), "Bennu");
  EXPECT_EQ((int) instGroup.findKeyword("SampleDetectors"), 1050);
  EXPECT_EQ((int) instGroup.findKeyword("LineDetectors"), 1050);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentType"), "FRAMING");
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("EphemerisTime"), 610696049.67661);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("StartTime"), "2019-05-09T17:46:20.424");
  EXPECT_EQ((int) instGroup.findKeyword("FocalPlaneXDependency"), 2);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransX"), 1.0);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransY"), 1.0);

  FileName matchedCubeName(instGroup.findKeyword("MatchedCube"));
  FileName inputCubeName(mCube.fileName());
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, matchedCubeName.name(), inputCubeName.name());
  
  
  // Call Skypt on an off-body sky pixel and check for valid pixel value.
  QVector<QString> argsSkypt1 = {"from="+noprojCube.fileName(),
                                 "format=pvl",
                                 "type=image", 
                                 "sample=20.0", 
                                 "line=30.0"};                
  UserInterface uiSkypt1(APP_XML1, argsSkypt1);
  Pvl appLogSkypt1;
  skypt(&noprojCube, uiSkypt1, &appLogSkypt1);
  PvlGroup skyPoint1 = appLogSkypt1.findGroup("SkyPoint");

  EXPECT_DOUBLE_EQ( (double) skyPoint1.findKeyword("Sample"), 20.0);
  EXPECT_DOUBLE_EQ( (double) skyPoint1.findKeyword("Line"), 30.0);
  EXPECT_NEAR( (double) skyPoint1.findKeyword("RightAscension"), 181.5703153396, 1e-8);
  EXPECT_NEAR( (double) skyPoint1.findKeyword("Declination"), -1.712838512527, 1e-8);
  EXPECT_NEAR( (double) skyPoint1.findKeyword("EphemerisTime"), 610696049.67661, 1e-8);
  EXPECT_NEAR( (double) skyPoint1.findKeyword("PixelValue"), 0.00034996954, 1e-8);
  EXPECT_NEAR( (double) skyPoint1.findKeyword("CelestialNorthClockAngle"), 239.06687506777001, 1e-8);

  // Call Skypt on an off-body pixel and check for a Null pixel value.
  QVector<QString> argsSkypt2 = {"from="+noprojCube.fileName(),
                                 "format=pvl",
                                 "type=image", 
                                 "sample=100.0", 
                                 "line=200.0"};                
  UserInterface uiSkypt2(APP_XML1, argsSkypt2);
  Pvl appLogSkypt2;
  skypt(&noprojCube, uiSkypt2, &appLogSkypt2);
  PvlGroup skyPoint2 = appLogSkypt2.findGroup("SkyPoint");
 
  // On-body pixel is expected to be valid.
  EXPECT_DOUBLE_EQ( (double) skyPoint2.findKeyword("Sample"), 100.0);
  EXPECT_DOUBLE_EQ( (double) skyPoint2.findKeyword("Line"), 200.0);
  EXPECT_NEAR( (double) skyPoint2.findKeyword("RightAscension"), 180.84505524757, 1e-8);
  EXPECT_NEAR( (double) skyPoint2.findKeyword("Declination"), -1.6394524986874, 1e-8);
  EXPECT_NEAR( (double) skyPoint2.findKeyword("EphemerisTime"), 610696049.67661, 1e-8);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, skyPoint2.findKeyword("PixelValue"), "Null");
  EXPECT_NEAR( (double) skyPoint2.findKeyword("CelestialNorthClockAngle"), 239.03639925925, 1e-8);

 // Call Skypt on an on-body pixel and check for valid pixel value.
  QVector<QString> argsSkypt3 = {"from="+noprojCube.fileName(),
                                 "format=pvl",
                                 "type=image", 
                                 "sample=500.0", 
                                 "line=500.0"};                
  UserInterface uiSkypt3(APP_XML1, argsSkypt3); 
  Pvl appLogSkypt3;
  skypt(&noprojCube, uiSkypt3, &appLogSkypt3);
  PvlGroup skyPoint3 = appLogSkypt3.findGroup("SkyPoint");
 
  // On-body pixel is expected to be valid.
  EXPECT_DOUBLE_EQ( (double) skyPoint3.findKeyword("Sample"), 500.0);
  EXPECT_DOUBLE_EQ( (double) skyPoint3.findKeyword("Line"), 500.0);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("RightAscension"), 179.04549552762, 1e-8);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("Declination"), -2.370616350111, 1e-8);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("EphemerisTime"), 610696049.67661, 1e-8);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("PixelValue"), 0.030219724, 1e-8);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("CelestialNorthClockAngle"), 238.96087237795, 1e-8);

}



/**
   * Test Noproj parameters OFFBODY=FALSE and OFFBODYTRIM=TRUE
   * Call Skypt to check that pixels in noprojed image 
   * have the expected values (Null or valid). 
   *
   * Input ...
   *   1) From cube, Level 1 cube, spiceinited, cropped MapCam cube.
   *   2) Match cube, Level 1 cube, spiceinited, cropped MapCam cube.
   * Output ...
   *   1) Noproj'ed cube.
   *   2) Pvl log file from Skypt.
   *
  */
 TEST_F(DefaultCube, FunctionalTestNoprojOffBodyFalseOffBodyTrimTrue) { 

  QString iCubeName = "data/osirisRexImages/20190509T180552S020_crop.cub";
  QString mCubeName = "data/osirisRexImages/20190509T174620S424_crop.cub";
  QString noprojCubeName = tempDir.path() + "/output.cub";
  QVector<QString> args = {"to=" + noprojCubeName, "offbody=FALSE", "offbodytrim=TRUE"};

  Cube iCube;
  Cube mCube;
  iCube.open(iCubeName);
  mCube.open(mCubeName);
  
  // Run noproj with match cube specified.
  UserInterface options(APP_XML, args); 
  noproj(&iCube, &mCube, options);

  // Check results of noproj cube.
  Cube noprojCube(noprojCubeName);
  Pvl *isisLabel = noprojCube.label();
  PvlGroup instGroup = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("SpacecraftName"), "IdealSpacecraft");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentId"), "IdealCamera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("TargetName"), "Bennu");
  EXPECT_EQ((int) instGroup.findKeyword("SampleDetectors"), 1050);
  EXPECT_EQ((int) instGroup.findKeyword("LineDetectors"), 1050);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentType"), "FRAMING");
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("EphemerisTime"), 610696049.67661);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("StartTime"), "2019-05-09T17:46:20.424");
  EXPECT_EQ((int) instGroup.findKeyword("FocalPlaneXDependency"), 2);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransX"), 1.0);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransY"), 1.0);

  FileName matchedCubeName(instGroup.findKeyword("MatchedCube"));
  FileName inputCubeName(mCube.fileName());
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, matchedCubeName.name(), inputCubeName.name());
  
  
  // Call Skypt on an off-body sky pixel and check for a Null pixel value.
  QVector<QString> argsSkypt1 = {"from="+noprojCube.fileName(),
                                 "format=pvl",
                                 "type=image", 
                                 "sample=20.0", 
                                 "line=30.0"};                
  UserInterface uiSkypt1(APP_XML1, argsSkypt1);
  Pvl appLogSkypt1;
  skypt(&noprojCube, uiSkypt1, &appLogSkypt1);
  PvlGroup skyPoint1 = appLogSkypt1.findGroup("SkyPoint");

  EXPECT_DOUBLE_EQ( (double) skyPoint1.findKeyword("Sample"), 20.0);
  EXPECT_DOUBLE_EQ( (double) skyPoint1.findKeyword("Line"), 30.0);
  EXPECT_NEAR( (double) skyPoint1.findKeyword("RightAscension"), 181.5703153396, 1e-8);
  EXPECT_NEAR( (double) skyPoint1.findKeyword("Declination"), -1.712838512527, 1e-8);
  EXPECT_NEAR( (double) skyPoint1.findKeyword("EphemerisTime"), 610696049.67661, 1e-8);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, skyPoint1.findKeyword("PixelValue"), "Null");
  EXPECT_NEAR( (double) skyPoint1.findKeyword("CelestialNorthClockAngle"), 239.06687506777, 1e-8);

  // Call Skypt on an off-body pixel and check for a Null pixel value.
  QVector<QString> argsSkypt2 = {"from="+noprojCube.fileName(),
                                 "format=pvl",
                                 "type=image", 
                                 "sample=100.0", 
                                 "line=200.0"};                
  UserInterface uiSkypt2(APP_XML1, argsSkypt2);
  Pvl appLogSkypt2;
  skypt(&noprojCube, uiSkypt2, &appLogSkypt2);
  PvlGroup skyPoint2 = appLogSkypt2.findGroup("SkyPoint");
 
  // On-body pixel is expected to be valid.
  EXPECT_DOUBLE_EQ( (double) skyPoint2.findKeyword("Sample"), 100.0);
  EXPECT_DOUBLE_EQ( (double) skyPoint2.findKeyword("Line"), 200.0);
  EXPECT_NEAR( (double) skyPoint2.findKeyword("RightAscension"), 180.84505524757, 1e-8);
  EXPECT_NEAR( (double) skyPoint2.findKeyword("Declination"), -1.639452498687, 1e-8);
  EXPECT_NEAR( (double) skyPoint2.findKeyword("EphemerisTime"), 610696049.67661, 1e-8);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, skyPoint2.findKeyword("PixelValue"), "Null");
  EXPECT_NEAR( (double) skyPoint2.findKeyword("CelestialNorthClockAngle"), 239.03639925925, 1e-8);

 // Call Skypt on an on-body pixel and check for valid pixel value.
  QVector<QString> argsSkypt3 = {"from="+noprojCube.fileName(),
                                 "format=pvl",
                                 "type=image", 
                                 "sample=500.0", 
                                 "line=500.0"};                
  UserInterface uiSkypt3(APP_XML1, argsSkypt3); 
  Pvl appLogSkypt3;
  skypt(&noprojCube, uiSkypt3, &appLogSkypt3);
  PvlGroup skyPoint3 = appLogSkypt3.findGroup("SkyPoint");
 
  // On-body pixel is expected to be valid.
  EXPECT_DOUBLE_EQ( (double) skyPoint3.findKeyword("Sample"), 500.0);
  EXPECT_DOUBLE_EQ( (double) skyPoint3.findKeyword("Line"), 500.0);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("RightAscension"), 179.04549552762001, 1e-8);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("Declination"), -2.370616350111, 1e-8);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("EphemerisTime"), 610696049.67661, 1e-8);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("PixelValue"), 0.030219724, 1e-8);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("CelestialNorthClockAngle"), 238.96087237795, 1e-8);

}



/**
   * Test Noproj parameters OFFBODY=FALSE and OFFBODYTRIM=FALSE
   * Call Skypt to check that pixels in noprojed image 
   * have the expected values (Null or valid). 
   *
   * Input ...
   *   1) From cube, Level 1 cube, spiceinited, cropped MapCam cube.
   *   2) Match cube, Level 1 cube, spiceinited, cropped MapCam cube.
   * Output ...
   *   1) Noproj'ed cube.
   *   2) Pvl log file from Skypt.
   *
  */
 TEST_F(DefaultCube, FunctionalTestNoprojOffBodyFalseOffBodyTrimFalse) { 

  QString iCubeName = "data/osirisRexImages/20190509T180552S020_crop.cub";
  QString mCubeName = "data/osirisRexImages/20190509T174620S424_crop.cub";
  QString noprojCubeName = tempDir.path() + "/output.cub";
  QVector<QString> args = {"to=" + noprojCubeName, "offbody=FALSE", "offbodytrim=FALSE"};

  Cube iCube;
  Cube mCube;
  iCube.open(iCubeName);
  mCube.open(mCubeName);
  
  // Run noproj with match cube specified.
  UserInterface options(APP_XML, args); 
  noproj(&iCube, &mCube, options);

  // Check results of noproj cube.
  Cube noprojCube(noprojCubeName);
  Pvl *isisLabel = noprojCube.label();
  PvlGroup instGroup = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("SpacecraftName"), "IdealSpacecraft");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentId"), "IdealCamera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("TargetName"), "Bennu");
  EXPECT_EQ((int) instGroup.findKeyword("SampleDetectors"), 1050);
  EXPECT_EQ((int) instGroup.findKeyword("LineDetectors"), 1050);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentType"), "FRAMING");
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("EphemerisTime"), 610696049.67661);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("StartTime"), "2019-05-09T17:46:20.424");
  EXPECT_EQ((int) instGroup.findKeyword("FocalPlaneXDependency"), 2);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransX"), 1.0);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransY"), 1.0);

  FileName matchedCubeName(instGroup.findKeyword("MatchedCube"));
  FileName inputCubeName(mCube.fileName());
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, matchedCubeName.name(), inputCubeName.name());
  
  
  // Call Skypt on an off-body sky pixel and check for a Null pixel value.
  QVector<QString> argsSkypt1 = {"from="+noprojCube.fileName(),
                                 "format=pvl",
                                 "type=image", 
                                 "sample=20.0", 
                                 "line=30.0"};                
  UserInterface uiSkypt1(APP_XML1, argsSkypt1);
  Pvl appLogSkypt1;
  skypt(&noprojCube, uiSkypt1, &appLogSkypt1);
  PvlGroup skyPoint1 = appLogSkypt1.findGroup("SkyPoint");

  EXPECT_DOUBLE_EQ( (double) skyPoint1.findKeyword("Sample"), 20.0);
  EXPECT_DOUBLE_EQ( (double) skyPoint1.findKeyword("Line"), 30.0);
  EXPECT_NEAR( (double) skyPoint1.findKeyword("RightAscension"), 181.5703153396, 1e-8);
  EXPECT_NEAR( (double) skyPoint1.findKeyword("Declination"), -1.712838512527, 1e-8);
  EXPECT_NEAR( (double) skyPoint1.findKeyword("EphemerisTime"), 610696049.67661, 1e-8);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, skyPoint1.findKeyword("PixelValue"), "Null");
  EXPECT_NEAR( (double) skyPoint1.findKeyword("CelestialNorthClockAngle"), 239.06687506777, 1e-8);

  // Call Skypt on an off-body pixel and check for a Null pixel value.
  QVector<QString> argsSkypt2 = {"from="+noprojCube.fileName(),
                                 "format=pvl",
                                 "type=image", 
                                 "sample=100.0", 
                                 "line=200.0"};                
  UserInterface uiSkypt2(APP_XML1, argsSkypt2);
  Pvl appLogSkypt2;
  skypt(&noprojCube, uiSkypt2, &appLogSkypt2);
  PvlGroup skyPoint2 = appLogSkypt2.findGroup("SkyPoint");
 
  // On-body pixel is expected to be valid.
  EXPECT_DOUBLE_EQ( (double) skyPoint2.findKeyword("Sample"), 100.0);
  EXPECT_DOUBLE_EQ( (double) skyPoint2.findKeyword("Line"), 200.0);
  EXPECT_NEAR( (double) skyPoint2.findKeyword("RightAscension"), 180.84505524757, 1e-8);
  EXPECT_NEAR( (double) skyPoint2.findKeyword("Declination"), -1.6394524986874, 1e-8);
  EXPECT_NEAR( (double) skyPoint2.findKeyword("EphemerisTime"), 610696049.67661, 1e-8);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, skyPoint2.findKeyword("PixelValue"), "Null");
  EXPECT_NEAR( (double) skyPoint2.findKeyword("CelestialNorthClockAngle"), 239.03639925925, 1e-8);

 // Call Skypt on an on-body pixel and check for valid pixel value.
  QVector<QString> argsSkypt3 = {"from="+noprojCube.fileName(),
                                 "format=pvl",
                                 "type=image", 
                                 "sample=500.0", 
                                 "line=500.0"};                
  UserInterface uiSkypt3(APP_XML1, argsSkypt3); 
  Pvl appLogSkypt3;
  skypt(&noprojCube, uiSkypt3, &appLogSkypt3);
  PvlGroup skyPoint3 = appLogSkypt3.findGroup("SkyPoint");
 
  // On-body pixel is expected to be valid.
  EXPECT_DOUBLE_EQ( (double) skyPoint3.findKeyword("Sample"), 500.0);
  EXPECT_DOUBLE_EQ( (double) skyPoint3.findKeyword("Line"), 500.0);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("RightAscension"), 179.04549552762, 1e-8);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("Declination"), -2.370616350112, 1e-8);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("EphemerisTime"), 610696049.67661, 1e-8);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("PixelValue"), 0.030219724, 1e-8);
  EXPECT_NEAR( (double) skyPoint3.findKeyword("CelestialNorthClockAngle"), 238.96087237795, 1e-8);

}

TEST_F(DefaultCube, FunctionalTestNoprojSn) {
  QString cubeFileName = tempDir.path() + "/output.cub";
  QVector<QString> args = {"to=" + cubeFileName};
  UserInterface options(APP_XML, args);
  
  noproj(testCube, NULL, options);

  Cube oCube(cubeFileName);
  Pvl *isisLabel = oCube.label();
  PvlGroup instGroup = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("SpacecraftName"), "IdealSpacecraft");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentId"), "IdealCamera");
  EXPECT_DOUBLE_EQ((double)instGroup.findKeyword("EphemerisTime"), -709401200.26114);

  // Get SN
  QString APP_XML_SN = FileName("$ISISROOT/bin/xml/getsn.xml").expanded();
  QVector<QString> sn_args = {"from=" + cubeFileName,
                              "file=true",
                              "observation=true"};
  UserInterface sn_options(APP_XML_SN, sn_args);
  Pvl appLog;
  getsn(sn_options, &appLog);

  PvlGroup results = appLog.findGroup("Results");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, results.findKeyword("Filename"), cubeFileName);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, results.findKeyword("SerialNumber"), "IdealSpacecraft/IdealCamera/-709401200.26114");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, results.findKeyword("ObservationNumber"), "IdealSpacecraft/IdealCamera/-709401200.26114");
  
}