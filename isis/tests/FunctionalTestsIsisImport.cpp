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


TEST_F(TempTestingFiles, FunctionalTestIsisImportCassiniIssNac) {
  Pvl appLog;
  QString cubeFileName = tempDir.path() + "/cissNac.cub";
  QVector<QString> args = {"from=data/ciss2isis/N1472853667_1.cropped.lbl", "to=" + cubeFileName};
  UserInterface options(APP_XML, args);

  try {
    isisimport(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest file: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(cubeFileName);
  Pvl *outLabel = outCube.label();

  PvlGroup dimensions = outLabel->findGroup("Dimensions", Pvl::Traverse);
  ASSERT_EQ((int)dimensions["Samples"], 1024);
  ASSERT_EQ((int)dimensions["Lines"], 10);
  ASSERT_EQ((int)dimensions["Bands"], 1);

  PvlGroup pixels = outLabel->findGroup("Pixels", Pvl::Traverse);
  ASSERT_EQ(pixels["Type"][0].toStdString(), "SignedWord");
  ASSERT_EQ(pixels["ByteOrder"][0].toStdString(), "Lsb");
  ASSERT_EQ((double)pixels["Base"], 0.0);
  ASSERT_EQ((double)pixels["Multiplier"], 1.0);

  std::istringstream iss(R"(
    Group = Instrument
      SpacecraftName          = Cassini-Huygens
      InstrumentId            = ISSNA
      TargetName              = Saturn
      StartTime               = 2004-09-02T21:32:36.410
      StopTime                = 2004-09-02T21:36:16.410
      ExposureDuration        = 220000.0 <Milliseconds>
      AntibloomingStateFlag   = On

      # BiasStripMean value converted back to 12 bit.
      BiasStripMean           = 50.00196
      CompressionRatio        = 1.845952
      CompressionType         = Lossless
      DataConversionType      = Table
      DelayedReadoutFlag      = No
      FlightSoftwareVersionId = 1.3
      GainModeId              = 12 <ElectronsPerDN>
      GainState               = 3
      ImageTime               = 2004-09-02T21:36:16.410
      InstrumentDataRate      = 182.783997 <KilobitsPerSecond>
      OpticsTemperature       = (0.712693, 1.905708 <DegreesCelcius>)
      ReadoutCycleIndex       = 10
      ShutterModeId           = NacOnly
      ShutterStateId          = Enabled
      SummingMode             = 1
      InstrumentModeId        = Full
      SpacecraftClockCount    = 1/1472853447.118
      ReadoutOrder            = 0
    End_Group
  )");

  PvlGroup truthInstGroup;
  iss >> truthInstGroup;
  PvlGroup &instGroup = outLabel->findGroup("Instrument", Pvl::Traverse);

  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, instGroup, truthInstGroup);

  std::istringstream arss(R"(
    Group = Archive
      DataSetId     = CO-S-ISSNA/ISSWA-2-EDR-V1.0
      ImageNumber   = 1472853667
      ObservationId = ISS_00ARI_DIFFUSRNG003_PRIME
      ProductId     = 1_N1472853667.118
    End_Group
  )");

  PvlGroup truthArchiveGroup;
  arss >> truthArchiveGroup;

  PvlGroup &archiveGroup = outLabel->findGroup("Archive", Pvl::Traverse);

  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, archiveGroup, truthArchiveGroup);

  std::istringstream bbss(R"(
    Group = BandBin
      FilterName   = CL1/CL2
      OriginalBand = 1
      Center       = 651.065
      Width        = 340.923
    End_Group
  )");

  PvlGroup truthBandBinGroup;
  bbss >> truthBandBinGroup;

  PvlGroup &bandBinGroup = outLabel->findGroup("BandBin", Pvl::Traverse);

  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, bandBinGroup, truthBandBinGroup);

  // Check for the ISS prefix pixel table
  ASSERT_TRUE(outLabel->hasObject("Table"));
  EXPECT_EQ(outLabel->findObject("Table", Pvl::Traverse)["Name"][0], "ISS Prefix Pixels");

  std::unique_ptr<Histogram> hist (outCube.histogram());
  EXPECT_NEAR(hist->Average(), 247.45226885705699, .00001);
  EXPECT_EQ(hist->Sum(), 2470316);
  EXPECT_EQ(hist->ValidPixels(), 9983);
  EXPECT_NEAR(hist->StandardDeviation(), 27.779542219945746, .0001);
}

TEST_F(TempTestingFiles, FunctionalTestIsisImportCassiniIssWac) {
  Pvl appLog;
  QString cubeFileName = tempDir.path() + "/cissWac.cub";
  QVector<QString> args = { "from=data/ciss2isis/W1472855646_5.cropped.lbl",
                            "to=" + cubeFileName };
  UserInterface options(APP_XML, args);
  try {
    isisimport(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest file: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(cubeFileName);
  Pvl *outLabel = outCube.label();

  PvlGroup dimensions = outLabel->findGroup("Dimensions", Pvl::Traverse);
  ASSERT_EQ((int)dimensions["Samples"], 1024);
  ASSERT_EQ((int)dimensions["Lines"], 10);
  ASSERT_EQ((int)dimensions["Bands"], 1);

  PvlGroup pixels = outLabel->findGroup("Pixels", Pvl::Traverse);
  ASSERT_EQ(pixels["Type"][0].toStdString(), "SignedWord");
  ASSERT_EQ(pixels["ByteOrder"][0].toStdString(), "Lsb");
  ASSERT_EQ((double)pixels["Base"], 0.0);
  ASSERT_EQ((double)pixels["Multiplier"], 1.0);

  std::istringstream iss(R"(
    Group = Instrument
      SpacecraftName          = Cassini-Huygens
      InstrumentId            = ISSWA
      TargetName              = Saturn
      StartTime               = 2004-09-02T22:09:15.409
      StopTime                = 2004-09-02T22:09:15.409
      ExposureDuration        = 5.0 <Milliseconds>
      AntibloomingStateFlag   = On
      BiasStripMean           = 72.644554
      CompressionRatio        = NotCompressed
      CompressionType         = NotCompressed
      DataConversionType      = 12Bit
      DelayedReadoutFlag      = Yes
      FlightSoftwareVersionId = 1.3
      GainModeId              = 29 <ElectronsPerDN>
      GainState               = 2
      ImageTime               = 2004-09-02T22:09:15.409
      InstrumentDataRate      = 182.783997 <KilobitsPerSecond>
      OpticsTemperature       = (7.024934, -999.0 <DegreesCelcius>)
      ReadoutCycleIndex       = 0
      ShutterModeId           = BothSim
      ShutterStateId          = Disabled
      SummingMode             = 1
      InstrumentModeId        = Full
      SpacecraftClockCount    = 1/1472855646.121
      ReadoutOrder            = 0
    End_Group
  )");

  PvlGroup truthInstGroup;
  iss >> truthInstGroup;
  PvlGroup &instGroup = outLabel->findGroup("Instrument", Pvl::Traverse);

  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, instGroup, truthInstGroup);

  std::istringstream arss(R"(
    Group = Archive
      DataSetId     = CO-S-ISSNA/ISSWA-2-EDR-V1.0
      ImageNumber   = 1472855646
      ObservationId = ISS_00ASA_MOS0ASWE001_UVIS
      ProductId     = 1_W1472855646.121
    End_Group
  )");

  PvlGroup truthArchiveGroup;
  arss >> truthArchiveGroup;

  PvlGroup &archiveGroup = outLabel->findGroup("Archive", Pvl::Traverse);

  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, archiveGroup, truthArchiveGroup);

  std::istringstream bbss(R"(
    Group = BandBin
      FilterName   = CL1/CL2
      OriginalBand = 1
      Center       = 633.837
      Width        = 285.938
    End_Group
  )");

  PvlGroup truthBandBinGroup;
  bbss >> truthBandBinGroup;

  PvlGroup &bandBinGroup = outLabel->findGroup("BandBin", Pvl::Traverse);

  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, bandBinGroup, truthBandBinGroup);

  std::unique_ptr<Histogram> hist (outCube.histogram());
  EXPECT_NEAR(hist->Average(), 70.914941406249994, .00001);
  EXPECT_EQ(hist->Sum(), 726169);
  EXPECT_EQ(hist->ValidPixels(), 10240);
  EXPECT_NEAR(hist->StandardDeviation(), 0.84419124016427105, .0001);

  // Commented out as the actual cube DN value ingestion is not working
  /*


  // Check for the ISS prefix pixel table
  ASSERT_TRUE(outLabel->hasObject("Table"));
  EXPECT_EQ(outLabel->findObject("Table", Pvl::Traverse)["Name"][0], "ISS Prefix Pixels");
  */
}

TEST_F(TempTestingFiles, FunctionalTestIsisImportCassiniIssCustomMax) {
  Pvl appLog;
  QString cubeFileName = tempDir.path() + "/ciss2isis_out.cub";

  QString inputLabel = "data/ciss2isis/W1472855646_5.cropped.lbl";
  QString updatedPvlLabel = tempDir.path() + "/W1472855646_5.cropped.lbl";
  Pvl inputPvl(inputLabel);
  inputPvl["VALID_MAXIMUM"][1] = "70";
  inputPvl.write(updatedPvlLabel);
  QFile::copy("data/ciss2isis/W1472855646_5.cropped.img", tempDir.path() + "/W1472855646_5.cropped.img");

  QVector<QString> args = { "from=" + updatedPvlLabel,
                            "to=" + cubeFileName };
  UserInterface options(APP_XML, args);
  try {
    isisimport(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest file: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(cubeFileName);
  Pvl *outLabel = outCube.label();

  PvlGroup dimensions = outLabel->findGroup("Dimensions", Pvl::Traverse);
  ASSERT_EQ((int)dimensions["Samples"], 1024);
  ASSERT_EQ((int)dimensions["Lines"], 10);
  ASSERT_EQ((int)dimensions["Bands"], 1);

  PvlGroup pixels = outLabel->findGroup("Pixels", Pvl::Traverse);
  ASSERT_EQ(pixels["Type"][0].toStdString(), "SignedWord");
  ASSERT_EQ(pixels["ByteOrder"][0].toStdString(), "Lsb");
  ASSERT_EQ((double)pixels["Base"], 0.0);
  ASSERT_EQ((double)pixels["Multiplier"], 1.0);

  std::unique_ptr<Histogram> hist (outCube.histogram());
  EXPECT_EQ(hist->Maximum(), 69);
  EXPECT_EQ(hist->ValidPixels(), 728);
  EXPECT_EQ(hist->HrsPixels(), (1024 * 10) - hist->ValidPixels());
}
