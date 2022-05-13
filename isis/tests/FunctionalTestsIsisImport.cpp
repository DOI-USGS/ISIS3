#include <iostream>
#include <time.h>

#include <QRegExp>
#include <QString>
#include <nlohmann/json.hpp>

#include "Fixtures.h"
#include "Histogram.h"
#include "md5wrapper.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "TestUtilities.h"

#include "isisimport.h"

#include "gmock/gmock.h"

using namespace Isis;
using namespace testing;
using json = nlohmann::json;

static QString APP_XML = FileName("$ISISROOT/bin/xml/isisimport.xml").expanded();

TEST_F(TempTestingFiles, FunctionalTestIsisImportLabelXmlInput) {
  QString labelFileName = "data/isisimport/pds4.xml";

  std::ofstream ofxml;
  ofxml.open(labelFileName.toStdString());
  ofxml << R"(<Dimensions> <Lines>2</Lines> <Samples>3</Samples> <Bands>1</Bands> </Dimensions>)";
  ofxml.close();

  QString templateFile = tempDir.path() + "/test_result.tpl";
  QString renderedCube = tempDir.path() + "/test_result.cub";

  std::ofstream of;
  of.open(templateFile.toStdString());
  of << R"(Object = IsisCube
  Object = Core
    Group = Dimensions
      Samples = {{Dimensions.Samples}}
      Lines   = {{Dimensions.Lines}}
      Bands   = {{Dimensions.Bands}}
    End_Group

    Group = Pixels
      Type       = Real
      ByteOrder  = Lsb
      Base       = 0.0
      Multiplier = 1.0
    End_Group
  End_Object
End_Object
Object = Translation
End_Object
End)";
  of.close();
  QVector<QString> args = {"from=" + labelFileName, "template=" + templateFile, "to=" + renderedCube};
  UserInterface options(APP_XML, args);

  isisimport(options);

  Cube cube;
  cube.open(renderedCube);
  Pvl *label = cube.label();
  PvlGroup dimensionsGroup = label->findObject("IsisCube").findObject("Core").findGroup("Dimensions");


  EXPECT_EQ(toInt(dimensionsGroup["Samples"][0]), 3);
  EXPECT_EQ(toInt(dimensionsGroup["Lines"][0]), 2);
  EXPECT_EQ(toInt(dimensionsGroup["Bands"][0]), 1);

  EXPECT_EQ(cube.sampleCount(), 3);
  EXPECT_EQ(cube.lineCount(), 2);
  EXPECT_EQ(cube.bandCount(), 1);

  EXPECT_EQ(cube.statistics()->Average(), 1);
  EXPECT_EQ(cube.statistics()->Minimum(), 1);
  EXPECT_EQ(cube.statistics()->Maximum(), 1);
  EXPECT_EQ(cube.statistics()->StandardDeviation(), 0);
}

TEST_F(TempTestingFiles, FunctionalTestIsisImportLabelPds4ErrorNoImage) {
  QString labelFileName = tempDir.path() + "/doesNotExist.xml";
  QString templateFile = tempDir.path() + "/test_result.tpl";
  QString renderedCube = tempDir.path() + "/test_result.cub";

  QVector<QString> args = {"from=" + labelFileName, "template=" + templateFile, "to=" + renderedCube};
  UserInterface options(APP_XML, args);
  EXPECT_ANY_THROW(isisimport(options));
}

TEST_F(TempTestingFiles, FunctionalTestIsisImportLabelPds4RemoveStartTimeZ) {
  QString labelFileName = "data/isisimport/pds4.xml";
  std::ofstream ofxml;
  ofxml.open(labelFileName.toStdString());
  ofxml << R"(<Cube><Dimensions> <Lines>2</Lines> <Samples>3</Samples> <Bands>1</Bands> </Dimensions><StartTime>2021-01-01T00:00:00Z</StartTime></Cube>)";
  ofxml.close();

  QString templateFile = tempDir.path() + "/test_result.tpl";
  QString renderedCube = tempDir.path() + "/test_result.cub";

  std::ofstream of;
  of.open(templateFile.toStdString());
  of << R"(Object = IsisCube
  Object = Core
    Group = Dimensions
      Samples = {{Cube.Dimensions.Samples}}
      Lines   = {{Cube.Dimensions.Lines}}
      Bands   = {{Cube.Dimensions.Bands}}
    End_Group

    Group = Pixels
      Type       = Real
      ByteOrder  = Lsb
      Base       = 0.0
      Multiplier = 1.0
    End_Group
  End_Object
    Group = Instrument
      StartTime = {{RemoveStartTimeZ(Cube.StartTime)}}
    End_Group
End_Object
Object = Translation
End_Object
End)";
  of.close();
  QVector<QString> args = {"from=" + labelFileName, "template=" + templateFile, "to=" + renderedCube};
  UserInterface options(APP_XML, args);

  isisimport(options);

  Cube cube;
  cube.open(renderedCube);
  Pvl *label = cube.label();
  PvlGroup instrumentGroup = label->findObject("IsisCube").findGroup("Instrument");

  EXPECT_EQ(instrumentGroup["StartTime"][0].toStdString(), "2021-01-01T00:00:00");
}

TEST_F(TempTestingFiles, FunctionalTestIsisImportLabelPds4YearDoy) {
QString labelFileName = "data/isisimport/pds4.xml";
  std::ofstream ofxml;
  ofxml.open(labelFileName.toStdString());
  ofxml << R"(<Cube><Dimensions> <Lines>2</Lines> <Samples>3</Samples> <Bands>1</Bands> </Dimensions><StartTime>2021-02-01T00:00:00Z
</StartTime></Cube>)";
  ofxml.close();

  QString templateFile = tempDir.path() + "/test_result.tpl";
  QString renderedCube = tempDir.path() + "/test_result.cub";
  std::ofstream of;
  of.open(templateFile.toStdString());
  of << R"(Object = IsisCube
  Object = Core
    Group = Dimensions
      Samples = {{Cube.Dimensions.Samples}}
      Lines   = {{Cube.Dimensions.Lines}}
      Bands   = {{Cube.Dimensions.Bands}}
    End_Group

    Group = Pixels
      Type       = Real
      ByteOrder  = Lsb
      Base       = 0.0
      Multiplier = 1.0
    End_Group
  End_Object
    Group = Archive
      YearDoy = {{YearDoy(Cube.StartTime)}}
    End_Group
End_Object
Object = Translation
End_Object
End)";
  of.close();
  QVector<QString> args = {"from=" + labelFileName, "template=" + templateFile, "to=" + renderedCube};
  UserInterface options(APP_XML, args);

  isisimport(options);

  Cube cube;
  cube.open(renderedCube);
  Pvl *label = cube.label();
  PvlGroup archiveGroup = label->findObject("IsisCube").findGroup("Archive");

  EXPECT_EQ(archiveGroup["YearDoy"][0].toStdString(), "202132");
}

TEST_F(TempTestingFiles, FunctionalTestIsisImportLabelObservationId) {
QString labelFileName = "data/isisimport/pds4.xml";
  std::ofstream ofxml;
  ofxml.open(labelFileName.toStdString());
  ofxml << R"(<Cube><Dimensions> <Lines>2</Lines> <Samples>3</Samples> <Bands>1</Bands> </Dimensions><UniqueIdentifier>2021
</UniqueIdentifier><Target>Mars</Target></Cube>)";
  ofxml.close();

  QString templateFile = tempDir.path() + "/test_result.tpl";
  QString renderedCube = tempDir.path() + "/test_result.cub";
  std::ofstream of;
  of.open(templateFile.toStdString());
  of << R"(Object = IsisCube
  Object = Core
    Group = Dimensions
      Samples = {{Cube.Dimensions.Samples}}
      Lines   = {{Cube.Dimensions.Lines}}
      Bands   = {{Cube.Dimensions.Bands}}
    End_Group

    Group = Pixels
      Type       = Real
      ByteOrder  = Lsb
      Base       = 0.0
      Multiplier = 1.0
    End_Group
  End_Object
    Group = Archive
      ObservationId = {{UniqueIdtoObservId(Cube.UniqueIdentifier, Cube.Target)}}
    End_Group
End_Object
Object = Translation
End_Object
End)";
  of.close();
  QVector<QString> args = {"from=" + labelFileName, "template=" + templateFile, "to=" + renderedCube};
  UserInterface options(APP_XML, args);

  isisimport(options);

  Cube cube;
  cube.open(renderedCube);
  Pvl *label = cube.label();
  PvlGroup archiveGroup = label->findObject("IsisCube").findGroup("Archive");

  EXPECT_EQ(archiveGroup["ObservationId"][0].toStdString(), "CRUS_000000_505_1");
}


TEST_F(TempTestingFiles, FunctionalTestIsisImportKaguyamiVis) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/kaguyatc2isisTEMP.cub";
  QVector<QString> args = {"from=data/kaguyami2isis/vis_cropped.img", "to="+cubeFileName};

  UserInterface options(APP_XML, args);
  try {
    isisimport(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest Kaguya MI image: " <<e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Dimensions group
  EXPECT_EQ(cube.sampleCount(), 962);
  EXPECT_EQ(cube.lineCount(), 20);
  EXPECT_EQ(cube.bandCount(), 2);

  // Pixels group
  EXPECT_EQ(PixelTypeName(cube.pixelType()), "SignedWord");
  EXPECT_EQ(ByteOrderName(cube.byteOrder()), "Lsb");
  EXPECT_DOUBLE_EQ(cube.base(), 0.0);
  EXPECT_DOUBLE_EQ(cube.multiplier(), 0.013);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_EQ(inst["MissionName"][0].toStdString(), "SELENE");
  EXPECT_EQ(inst["SpacecraftName"][0].toStdString(), "KAGUYA");
  EXPECT_EQ(inst["InstrumentName"][0].toStdString(), "Multiband Imager Visible");
  EXPECT_EQ(inst["InstrumentId"][0].toStdString(), "MI-VIS");
  EXPECT_EQ(inst["TargetName"][0].toStdString(), "MOON");
  EXPECT_EQ(inst["StartTime"][0].toStdString(), "2008-09-16T20:11:04.162607");
  EXPECT_EQ(inst["StopTime"][0].toStdString(), "2008-09-16T20:11:16.629582");
  EXPECT_EQ(inst["SpacecraftClockStartCount"][0].toStdString(), "905631054.826");
  EXPECT_EQ(inst["SpacecraftClockStopCount"][0].toStdString(), "905631067.294");
  EXPECT_DOUBLE_EQ(inst["LineExposureDuration"], 2.6623);
  EXPECT_EQ(inst["LineExposureDuration"].unit(), "msec");
  EXPECT_DOUBLE_EQ(inst["LineSamplingInterval"], 13);
  EXPECT_EQ(inst["LineSamplingInterval"].unit(), "msec");
  EXPECT_DOUBLE_EQ(inst["CorrectedSamplingInterval"], 12.999974);
  EXPECT_EQ(inst["CorrectedSamplingInterval"].unit(), "msec");

  // Archive Group
  PvlGroup &arch = isisLabel->findGroup("Archive", Pvl::Traverse);

  EXPECT_EQ(arch["DataSetId"][0].toStdString(), "MI-VIS_Level2B");
  EXPECT_EQ(arch["ProductSetId"][0].toStdString(), "MI-VIS_Level2B2");

  // Bandbin Group
  PvlGroup &bandBin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  std::istringstream bandBinStream(R"(
  Group = BandBin
    FilterName = (MV1, MV2, MV3, MV4, MV5)
    Center     = (414.0, 749.0, 901.0, 950.0, 1001.0) <nm>
    Width      = (20.0, 12.0, 21.0, 30.0, 42.0) <nm>
    BaseBand   = MV5
  End_Group
  )");
  PvlGroup bandBinTruth;
  bandBinStream >> bandBinTruth;
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, bandBin, bandBinTruth);

  // Kernels Group
  PvlGroup &kern = isisLabel->findGroup("Kernels", Pvl::Traverse);
  EXPECT_EQ(int(kern["NaifCkCode"]), -131330);
  EXPECT_EQ(int(kern["NaifFrameCode"]), -131335);

  std::unique_ptr<Histogram> hist (cube.histogram());

  EXPECT_DOUBLE_EQ(hist->Average(), 25.685768243243238);
  EXPECT_DOUBLE_EQ(hist->Sum(), 494194.18099999992);
  EXPECT_EQ(hist->ValidPixels(), 19240);
  EXPECT_DOUBLE_EQ(hist->StandardDeviation(), 26.830242572528928);
}

TEST_F(TempTestingFiles, FunctionalTestIsisImportKaguyamiNir) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/kaguyatc2isisTEMP.cub";
  QVector<QString> args = {"from=data/kaguyami2isis/nir_cropped.img", "to="+cubeFileName};

  UserInterface options(APP_XML, args);
  try {
    isisimport(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest Kaguya MI image: " <<e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Dimensions group
  EXPECT_EQ(cube.sampleCount(), 320);
  EXPECT_EQ(cube.lineCount(), 20);
  EXPECT_EQ(cube.bandCount(), 2);

  // Pixels group
  EXPECT_EQ(PixelTypeName(cube.pixelType()), "SignedWord");
  EXPECT_EQ(ByteOrderName(cube.byteOrder()), "Lsb");
  EXPECT_DOUBLE_EQ(cube.base(), 0.0);
  EXPECT_DOUBLE_EQ(cube.multiplier(), 0.013);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_EQ(inst["MissionName"][0].toStdString(), "SELENE");
  EXPECT_EQ(inst["SpacecraftName"][0].toStdString(), "KAGUYA");
  EXPECT_EQ(inst["InstrumentName"][0].toStdString(), "Multiband Imager Near Infrared");
  EXPECT_EQ(inst["InstrumentId"][0].toStdString(), "MI-NIR");
  EXPECT_EQ(inst["TargetName"][0].toStdString(), "MOON");
  EXPECT_EQ(inst["StartTime"][0].toStdString(), "2008-09-16T20:10:30.480257");
  EXPECT_EQ(inst["StopTime"][0].toStdString(), "2008-09-16T20:10:42.921232");
  EXPECT_EQ(inst["SpacecraftClockStartCount"][0].toStdString(), "905631021.132");
  EXPECT_EQ(inst["SpacecraftClockStopCount"][0].toStdString(), "905631033.574");
  EXPECT_DOUBLE_EQ(inst["LineExposureDuration"], 13.2148);
  EXPECT_EQ(inst["LineExposureDuration"].unit(), "msec");
  EXPECT_DOUBLE_EQ(inst["LineSamplingInterval"], 39);
  EXPECT_EQ(inst["LineSamplingInterval"].unit(), "msec");
  EXPECT_DOUBLE_EQ(inst["CorrectedSamplingInterval"], 38.999922);
  EXPECT_EQ(inst["CorrectedSamplingInterval"].unit(), "msec");

  // Archive Group
  PvlGroup &arch = isisLabel->findGroup("Archive", Pvl::Traverse);

  EXPECT_EQ(arch["DataSetId"][0].toStdString(), "MI-NIR_Level2B");
  EXPECT_EQ(arch["ProductSetId"][0].toStdString(), "MI-NIR_Level2B2");

  // Bandbin Group
  PvlGroup &bandBin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  std::istringstream bandBinStream(R"(
  Group = BandBin
    FilterName = (MN1, MN2, MN3, MN4)
    Center     = (1000.0, 1049.0, 1248.0, 1548.0) <nm>
    Width      = (27.0, 28.0, 33.0, 48.0) <nm>
    BaseBand   = MN1
  End_Group
  )");
  PvlGroup bandBinTruth;
  bandBinStream >> bandBinTruth;
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, bandBin, bandBinTruth);

  // Kernels Group
  PvlGroup &kern = isisLabel->findGroup("Kernels", Pvl::Traverse);
  EXPECT_EQ(int(kern["NaifCkCode"]), -131340);
  EXPECT_EQ(int(kern["NaifFrameCode"]), -131341);

  std::unique_ptr<Histogram> hist (cube.histogram());

  EXPECT_DOUBLE_EQ(hist->Average(), 29.395262812500022);
  EXPECT_DOUBLE_EQ(hist->Sum(), 188129.68200000015);
  EXPECT_EQ(hist->ValidPixels(), 6400);
  EXPECT_DOUBLE_EQ(hist->StandardDeviation(), 2.8449125231835715);
}

TEST_F(TempTestingFiles, FunctionalTestIsisImportKaguyamiProj) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/kaguyatc2isisTEMP.cub";
  QVector<QString> args = {"from=data/kaguyami2isis/3C5_label.pvl", "to="+cubeFileName};

  UserInterface options(APP_XML, args);
  try {
    isisimport(options);
    FAIL() << "Should not have been able to ingest: " << args[0].toStdString().c_str() << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("**PROGRAMMER ERROR** Unable to create a cube label from"));
  }
}
/*
TEST_F(TempTestingFiles, FunctionalTestIsisImportKaguyamiNullRange) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "kaguyatc2isisTEMP.cub";
  QVector<QString> args = {"from=data/kaguyami2isis/vis_cropped.img",
                           "setnullrange=yes",
                           "nullmin=0",
                           "nullmax=17486",
                           "to="+cubeFileName};

  UserInterface options(APP_XML, args);
  try {
    isisimport(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest Kaguya MI image: " <<e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  std::unique_ptr<Histogram> hist (cube.histogram());

  EXPECT_EQ(hist->ValidPixels(), 79);
  EXPECT_EQ(hist->NullPixels(), 19161);
}

TEST_F(TempTestingFiles, FunctionalTestIsisImportKaguyamiHrsRange) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "kaguyatc2isisTEMP.cub";
  QVector<QString> args = {"from=data/kaguyami2isis/vis_cropped.img",
                           "sethrsrange=yes",
                           "hrsmin=0",
                           "hrsmax=17486",
                           "to="+cubeFileName};

  UserInterface options(APP_XML, args);
  try {
    isisimport(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest Kaguya MI image: " <<e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  std::unique_ptr<Histogram> hist (cube.histogram());

  EXPECT_EQ(hist->ValidPixels(), 79);
  EXPECT_EQ(hist->HrsPixels(), 19161);
}

TEST_F(TempTestingFiles, FunctionalTestIsisImportKaguyamiHisRange) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "kaguyatc2isisTEMP.cub";
  QVector<QString> args = {"from=data/kaguyami2isis/vis_cropped.img",
                           "sethisrange=yes",
                           "hismin=0",
                           "hismax=17486",
                           "to="+cubeFileName};

  UserInterface options(APP_XML, args);
  try {
    isisimport(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest Kaguya MI image: " <<e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  std::unique_ptr<Histogram> hist (cube.histogram());

  EXPECT_EQ(hist->ValidPixels(), 79);
  EXPECT_EQ(hist->HisPixels(), 19161);
}

TEST_F(TempTestingFiles, FunctionalTestIsisImportKaguyamiLrsRange) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "kaguyatc2isisTEMP.cub";
  QVector<QString> args = {"from=data/kaguyami2isis/vis_cropped.img",
                           "setlrsrange=yes",
                           "lrsmin=0",
                           "lrsmax=17486",
                           "to="+cubeFileName};

  UserInterface options(APP_XML, args);
  try {
    isisimport(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest Kaguya MI image: " <<e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  std::unique_ptr<Histogram> hist (cube.histogram());

  EXPECT_EQ(hist->ValidPixels(), 79);
  EXPECT_EQ(hist->LrsPixels(), 19161);
}

TEST_F(TempTestingFiles, FunctionalTestIsisImportKaguyamiLisRange) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "kaguyatc2isisTEMP.cub";
  QVector<QString> args = {"from=data/kaguyami2isis/vis_cropped.img",
                           "setlisrange=yes",
                           "lismin=0",
                           "lismax=17486",
                           "to="+cubeFileName};

  UserInterface options(APP_XML, args);
  try {
    isisimport(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest Kaguya MI image: " <<e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  std::unique_ptr<Histogram> hist (cube.histogram());

  EXPECT_EQ(hist->ValidPixels(), 79);
  EXPECT_EQ(hist->LisPixels(), 19161);
}
*/
TEST_F(TempTestingFiles, FunctionalTestIsisImportKaguyamiError) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "kaguyatc2isisTEMP.cub";
  QVector<QString> args = {"from=data/kaguyatc2isis/TC1S2B0_01_05186N225E0040_mini.lbl",
                           "to="+cubeFileName};

  UserInterface options(APP_XML, args);
  try {
   isisimport(options);
   FAIL() << "Should not have been able to ingest: " << args[0].toStdString().c_str() << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("**PROGRAMMER ERROR** Unable to create a cube label from"));
  }
}

TEST(IsisImportTest, loTestDefault) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/lo2isisTEMP.cub";
  QVector<QString> args = {"from=data/lo2isis/case01/3133_h1_cropped.cub", "to="+cubeFileName};

  UserInterface options(APP_XML, args);
  try {
    isisimport(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest LO image: " <<e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Dimensions group
  ASSERT_EQ(cube.sampleCount(), 151);
  ASSERT_EQ(cube.lineCount(), 5);
  ASSERT_EQ(cube.bandCount(), 1);

  // Pixels group
  ASSERT_EQ(PixelTypeName(cube.pixelType()), "UnsignedByte");
  ASSERT_EQ(ByteOrderName(cube.byteOrder()), "Lsb");
  ASSERT_DOUBLE_EQ(cube.base(), 0.0);
  ASSERT_DOUBLE_EQ(cube.multiplier(), 1.0);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  ASSERT_EQ(inst["SpacecraftName"][0].toStdString(), "Lunar Orbiter 3");
  ASSERT_EQ(inst["InstrumentId"][0].toStdString(), "High Resolution Camera");
  ASSERT_EQ(inst["TargetName"][0].toStdString(), "Moon");
  ASSERT_EQ(inst["StartTime"][0].toStdString(), "1967-02-20T08:14:28.610");
  ASSERT_DOUBLE_EQ(inst["FiducialCoordinateMicron"], 50);
  ASSERT_EQ(inst["FiducialCoordinateMicron"].unit(), "um");
  ASSERT_DOUBLE_EQ(inst["FrameNumber"], 3133);

  ASSERT_EQ(inst["FiducialID"][0].toStdString(), "1b");
  ASSERT_EQ(inst["FiducialID"][6].toStdString(), "73a");
  ASSERT_EQ(inst["FiducialID"][14].toStdString(), "144b");
  ASSERT_EQ(inst["FiducialID"][29].toStdString(), "283b");

  ASSERT_DOUBLE_EQ(inst["FiducialSamples"][0].toDouble(), 32162.0);
  ASSERT_DOUBLE_EQ(inst["FiducialSamples"][6].toDouble(), 24295.0);
  ASSERT_DOUBLE_EQ(inst["FiducialSamples"][14].toDouble(), 16593.0);
  ASSERT_DOUBLE_EQ(inst["FiducialSamples"][29].toDouble(), 1248.0);

  ASSERT_DOUBLE_EQ(inst["FiducialLines"][0].toDouble(), 8510.0);
  ASSERT_DOUBLE_EQ(inst["FiducialLines"][6].toDouble(), 8504.0);
  ASSERT_DOUBLE_EQ(inst["FiducialLines"][14].toDouble(), 584.0);
  ASSERT_DOUBLE_EQ(inst["FiducialLines"][29].toDouble(), 8496.0);

  ASSERT_DOUBLE_EQ(inst["FiducialXCoordinates"][0].toDouble(), -108.168);
  ASSERT_DOUBLE_EQ(inst["FiducialXCoordinates"][6].toDouble(), -53.474);
  ASSERT_DOUBLE_EQ(inst["FiducialXCoordinates"][14].toDouble(), 0.122);
  ASSERT_DOUBLE_EQ(inst["FiducialXCoordinates"][29].toDouble(), 106.844);

  ASSERT_DOUBLE_EQ(inst["FiducialYCoordinates"][0].toDouble(), 27.476);
  ASSERT_DOUBLE_EQ(inst["FiducialYCoordinates"][6].toDouble(), 27.5);
  ASSERT_DOUBLE_EQ(inst["FiducialYCoordinates"][14].toDouble(), -27.493);
  ASSERT_DOUBLE_EQ(inst["FiducialYCoordinates"][29].toDouble(), 27.479);

  // Bandbin Group
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  ASSERT_EQ(bandbin["Center"][0], "1.0");
  ASSERT_EQ(bandbin["OriginalBand"][0], "1");

  // Kernels Group
  PvlGroup &kern = isisLabel->findGroup("Kernels", Pvl::Traverse);
  ASSERT_EQ(int(kern["NaifFrameCode"]), -533001);

  std::unique_ptr<Histogram> hist (cube.histogram());

  ASSERT_NEAR(hist->Average(), 75.43576, .00001);
  ASSERT_EQ(hist->Sum(), 56954);
  ASSERT_EQ(hist->ValidPixels(), 755);
  ASSERT_NEAR(hist->StandardDeviation(), 11.2905, .0001);
}

TEST(IsisImportTest, loMirrored) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/lo2isisTEMP.cub";
  QVector<QString> args = {"from=data/lo2isis/case02/4164H_Full_mirror_cropped.cub", "to="+cubeFileName};

  UserInterface options(APP_XML, args);
  try {
    isisimport(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest LO image: " <<e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Dimensions group
  ASSERT_EQ(cube.sampleCount(), 34530);
  ASSERT_EQ(cube.lineCount(), 5);
  ASSERT_EQ(cube.bandCount(), 1);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  ASSERT_EQ(inst["SpacecraftName"][0].toStdString(), "Lunar Orbiter 4");
  ASSERT_EQ(inst["InstrumentId"][0].toStdString(), "High Resolution Camera");
  ASSERT_EQ(inst["TargetName"][0].toStdString(), "Moon");
  ASSERT_EQ(inst["StartTime"][0].toStdString(), "1967-05-23T07:12:45.810");
  ASSERT_DOUBLE_EQ(inst["FrameNumber"], 4164);

  // Bandbin Group
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  ASSERT_EQ(bandbin["Center"][0], "1.0");
  ASSERT_EQ(bandbin["OriginalBand"][0], "1");

  // Kernels Group
  PvlGroup &kern = isisLabel->findGroup("Kernels", Pvl::Traverse);
  ASSERT_EQ(int(kern["NaifFrameCode"]), -534001);
}

TEST(IsisImportTest, loMedToHi) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/lo2isisTEMP.cub";
  QVector<QString> args = {"from=data/lo2isis/case03/3083_med_tohi_isis2_cropped.cub", "to="+cubeFileName};

  UserInterface options(APP_XML, args);
  try {
    isisimport(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest LO image: " <<e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Dimensions group
  ASSERT_EQ(cube.sampleCount(), 100);
  ASSERT_EQ(cube.lineCount(), 5);
  ASSERT_EQ(cube.bandCount(), 1);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  ASSERT_EQ(inst["SpacecraftName"][0].toStdString(), "Lunar Orbiter 3");
  ASSERT_EQ(inst["InstrumentId"][0].toStdString(), "Medium Resolution Camera");
  ASSERT_EQ(inst["TargetName"][0].toStdString(), "Moon");
  ASSERT_EQ(inst["StartTime"][0].toStdString(), "1967-02-17T21:09:27.610");
  ASSERT_DOUBLE_EQ(inst["FrameNumber"], 3083);
  ASSERT_DOUBLE_EQ(inst["BoresightSample"], 5427.039);
  ASSERT_DOUBLE_EQ(inst["BoresightLine"], 4550.455);
  ASSERT_DOUBLE_EQ(inst["SubFrame"], 0);

  // Bandbin Group
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  ASSERT_EQ(bandbin["FilterName"][0].toStdString(), "none");
  ASSERT_EQ(bandbin["Center"][0], "1.0");
  ASSERT_EQ(bandbin["OriginalBand"][0], "1");

  // Kernels Group
  PvlGroup &kern = isisLabel->findGroup("Kernels", Pvl::Traverse);
  ASSERT_EQ(int(kern["NaifFrameCode"]), -533002);
}

TEST(IsisImportTest, loMed) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/lo2isisTEMP.cub";
  QVector<QString> args = {"from=data/lo2isis/case04/3083_med_isis2_cropped.cub", "to="+cubeFileName};

  UserInterface options(APP_XML, args);
  try {
    isisimport(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest LO image: " <<e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Dimensions group
  ASSERT_EQ(cube.sampleCount(), 11800);
  ASSERT_EQ(cube.lineCount(), 5);
  ASSERT_EQ(cube.bandCount(), 1);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  ASSERT_EQ(inst["SpacecraftName"][0].toStdString(), "Lunar Orbiter 3");
  ASSERT_EQ(inst["InstrumentId"][0].toStdString(), "Medium Resolution Camera");
  ASSERT_EQ(inst["TargetName"][0].toStdString(), "Moon");
  ASSERT_EQ(inst["StartTime"][0].toStdString(), "1967-02-17T21:09:27.610");
  ASSERT_DOUBLE_EQ(inst["FrameNumber"], 3083);
  ASSERT_DOUBLE_EQ(inst["SubFrame"], 0);

  // Bandbin Group
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  ASSERT_EQ(bandbin["FilterName"][0].toStdString(), "none");
  ASSERT_EQ(bandbin["Center"][0], "1.0");
  ASSERT_EQ(bandbin["OriginalBand"][0], "1");

  // Kernels Group
  PvlGroup &kern = isisLabel->findGroup("Kernels", Pvl::Traverse);
  ASSERT_EQ(int(kern["NaifFrameCode"]), -533002);
}

TEST(IsisImportTest, loReingest) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/lo2isisTEMP.cub";
  QVector<QString> args = {"from=data/lo2isis/reimport/3133_h1.pds_cropped.img", "to="+cubeFileName};

  UserInterface options(APP_XML, args);
  try {
    isisimport(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest LO image: " <<e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Dimensions group
  ASSERT_EQ(cube.sampleCount(), 151);
  ASSERT_EQ(cube.lineCount(), 5);
  ASSERT_EQ(cube.bandCount(), 1);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  ASSERT_EQ(inst["SpacecraftName"][0].toStdString(), "Lunar Orbiter 3");
  ASSERT_EQ(inst["InstrumentId"][0].toStdString(), "High Resolution Camera");
  ASSERT_EQ(inst["TargetName"][0].toStdString(), "Moon");
  ASSERT_EQ(inst["StartTime"][0].toStdString(), "1967-02-20T08:14:28.610000");
  ASSERT_DOUBLE_EQ(inst["FrameNumber"], 3133);
  ASSERT_DOUBLE_EQ(inst["SubFrame"], 2921);

  // Kernels Group
  PvlGroup &kern = isisLabel->findGroup("Kernels", Pvl::Traverse);
  ASSERT_EQ(int(kern["NaifFrameCode"]), -533001);
}
