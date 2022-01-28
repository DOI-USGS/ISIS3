#include <iostream>
#include <time.h>

#include <QRegExp>
#include <QString>
#include <nlohmann/json.hpp>

#include "Fixtures.h"
#include "md5wrapper.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"

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





TEST_F(TempTestingFiles, FunctionalTestIsisImportKaguyaTC) {
  std::istringstream PvlInput(R"(
                      Object = IsisCube
                      Object = Core
                        StartByte   = 65537
                        Format      = Tile
                        TileSamples = 1024
                        TileLines   = 1024

                        Group = Dimensions
                          Samples = 3
                          Lines   = 2
                          Bands   = 1
                        End_Group

                        Group = Pixels
                          Type       = SignedWord
                          ByteOrder  = Lsb
                          Base       = 0.0
                          Multiplier = 2.0e-05
                        End_Group
                      End_Object

                      Group = Instrument
                        MissionName        = SELENE
                        SpacecraftName     = KAGUYA
                        InstrumentName     = "Terrain Camera"
                        InstrumentId       = TC
                        TargetName         = MOON
                        ObservationModeId  = NORMAL&SUPPORT
                        SensorDescription  = "Imagery type:Pushbroom. ImageryMode:Mono,Stereo.
                                              ExposureTimeMode:Long,Middle,Short.
                                              CompressionMode:NonComp,DCT. Q-table:32 patterns.
                                              H-table:4 patterns.
                                              SwathMode:F(Full),N(Nominal),H(Half). First pixel
                                              number:1(F),297(N),1172(H)."
                        SensorDescription2 = "Pixel size:7x7[micron^2](TC1/TC2). Wavelength
                                              range:430-850[nm](TC1/TC2). A/D
                                              rate:10[bit](TC1/TC2). Slant angle:+/-15[degree]
                                              (from nadir to +x of S/C)(TC1/TC2). Focal
                                              length:72.45/72.63[mm](TC1/TC2). F
                                              number:3.97/3.98(TC1/TC2)."
                      End_Group

                      Group = Archive
                        ProductId                   = TC_EVE_02_S09E000S12E003SC
                        SoftwareName                = RGC_TC_MI
                        SoftwareVersion             = 2.5.0
                        ProcessVersionId            = MAP
                        ProductCreationTime         = 2009-09-17T01:03:48Z
                        ProgramStartTime            = 2009-09-17T00:58:53Z
                        ProducerId                  = LISM
                        ProductSetId                = TC_Evening_MAP
                        ProductVersionId            = 02
                        RegisteredProduct           = Y
                        Level2AFileName             = (TC2S2A0_02DMF03519_003_0012.img,
                                                      TC2S2A0_02DMF03519_003_0013.img,
                                                      TC2S2A0_02DMF03519_003_0014.img,
                                                      TC2S2A0_02DMF03520_003_0012.img,
                                                      TC2S2A0_02DMF03520_003_0013.img,
                                                      TC2S2A0_02DMF03520_003_0014.img,
                                                      TC2S2A0_02DMF03521_003_0012.img,
                                                      TC2S2A0_02DMF03521_003_0013.img,
                                                      TC2S2A0_02DMF03521_003_0014.img,
                                                      TC2S2A0_02DMF03522_003_0012.img,
                                                      TC2S2A0_02DMF03522_003_0013.img,
                                                      TC2S2A0_02DMF03522_003_0014.img)
                        SpiceMetakernelFileName     = (RGC_INF_TCv301IK_RISE100g_de421_090819.mk,
                                                      RGC_INF_TCv301IK_RISE100g_de421_090819.mk,
                                                      RGC_INF_TCv301IK_RISE100g_de421_090819.mk,
                                                      RGC_INF_TCv301IK_RISE100g_de421_090819.mk,
                                                      RGC_INF_TCv301IK_RISE100g_de421_090819.mk,
                                                      RGC_INF_TCv301IK_RISE100g_de421_090819.mk,
                                                      RGC_INF_TCv301IK_RISE100g_de421_090819.mk,
                                                      RGC_INF_TCv301IK_RISE100g_de421_090819.mk,
                                                      RGC_INF_TCv301IK_RISE100g_de421_090819.mk,
                                                      RGC_INF_TCv301IK_RISE100g_de421_090819.mk,
                                                      RGC_INF_TCv301IK_RISE100g_de421_090819.mk,
                                                      RGC_INF_TCv301IK_RISE100g_de421_090819.mk)
                        DataSetId                   = TC_MAP
                        ImageValueType              = REFLECTANCE
                        ImageUnit                   = ND
                        MinForStatisticalEvaluation = 0
                        MaxForStatisticalEvaluation = 32767
                        SceneMaximumDn              = 25005
                        SceneMinimumDn              = 1
                        SceneAverageDn              = 3782.5
                        SceneStdevDn                = 1777.9
                        SceneModeDn                 = 154
                        ShadowedAreaMinimum         = 0
                        ShadowedAreaMaximum         = 0
                        ShadowedAreaPercentage      = 0
                        InvalidType                 = (SATURATION, MINUS, DUMMY_DEFECT, OTHER)
                        InvalidValue                = (-20000, -21000, -22000, -23000)
                        InvalidPixels               = (0, 0, 0, 0)
                        OutOfImageBoundsValue       = -30000
                        OutOfImageBoundsPixel       = 0
                        StretchedFlag               = FALSE
                        DarkFileName                = (TC2_DRK_02952_04739_M_C_b05.csv,
                                                      TC2_DRK_02952_04739_M_C_b05.csv,
                                                      TC2_DRK_02952_04739_M_C_b05.csv,
                                                      TC2_DRK_02952_04739_M_C_b05.csv,
                                                      TC2_DRK_02952_04739_M_C_b05.csv,
                                                      TC2_DRK_02952_04739_M_C_b05.csv,
                                                      TC2_DRK_02952_04739_M_C_b05.csv,
                                                      TC2_DRK_02952_04739_M_C_b05.csv,
                                                      TC2_DRK_02952_04739_M_C_b05.csv,
                                                      TC2_DRK_02952_04739_M_C_b05.csv,
                                                      TC2_DRK_02952_04739_M_C_b05.csv,
                                                      TC2_DRK_02952_04739_M_C_b05.csv)
                        FlatFileName                = (TC2_FLT_00293_04739_N_C_b05.csv,
                                                      TC2_FLT_00293_04739_N_C_b05.csv,
                                                      TC2_FLT_00293_04739_N_C_b05.csv,
                                                      TC2_FLT_00293_04739_N_C_b05.csv,
                                                      TC2_FLT_00293_04739_N_C_b05.csv,
                                                      TC2_FLT_00293_04739_N_C_b05.csv,
                                                      TC2_FLT_00293_04739_N_C_b05.csv,
                                                      TC2_FLT_00293_04739_N_C_b05.csv,
                                                      TC2_FLT_00293_04739_N_C_b05.csv,
                                                      TC2_FLT_00293_04739_N_C_b05.csv,
                                                      TC2_FLT_00293_04739_N_C_b05.csv,
                                                      TC2_FLT_00293_04739_N_C_b05.csv)
                        EfficFileName               = (TC2_EFF_PRFLT_N_N_v01.csv,
                                                      TC2_EFF_PRFLT_N_N_v01.csv,
                                                      TC2_EFF_PRFLT_N_N_v01.csv,
                                                      TC2_EFF_PRFLT_N_N_v01.csv,
                                                      TC2_EFF_PRFLT_N_N_v01.csv,
                                                      TC2_EFF_PRFLT_N_N_v01.csv,
                                                      TC2_EFF_PRFLT_N_N_v01.csv,
                                                      TC2_EFF_PRFLT_N_N_v01.csv,
                                                      TC2_EFF_PRFLT_N_N_v01.csv,
                                                      TC2_EFF_PRFLT_N_N_v01.csv,
                                                      TC2_EFF_PRFLT_N_N_v01.csv,
                                                      TC2_EFF_PRFLT_N_N_v01.csv)
                        NonlinFileName              = (TC2_NLT_PRFLT_N_N_v01.csv,
                                                      TC2_NLT_PRFLT_N_N_v01.csv,
                                                      TC2_NLT_PRFLT_N_N_v01.csv,
                                                      TC2_NLT_PRFLT_N_N_v01.csv,
                                                      TC2_NLT_PRFLT_N_N_v01.csv,
                                                      TC2_NLT_PRFLT_N_N_v01.csv,
                                                      TC2_NLT_PRFLT_N_N_v01.csv,
                                                      TC2_NLT_PRFLT_N_N_v01.csv,
                                                      TC2_NLT_PRFLT_N_N_v01.csv,
                                                      TC2_NLT_PRFLT_N_N_v01.csv,
                                                      TC2_NLT_PRFLT_N_N_v01.csv,
                                                      TC2_NLT_PRFLT_N_N_v01.csv)
                        RadCnvCoef                  = (3.723389, 3.723389, 3.723389, 3.723389,
                                                      3.723389, 3.723389, 3.723389, 3.723389,
                                                      3.723389, 3.723389, 3.723389,
                                                      3.723389) <W/m**2/micron/sr>
                        RefCnvCoef                  = 0.002396 <1/(W/m**2/micron/sr)>
                        StandardGeometry            = (30.0, 0.0, 30.0)
                        PhotoCorrId                 = USGS
                        PhotoCorrCoef               = (-1.900000e-02, 2.420000e-04, -1.460000e-06,
                                                      5.400000e-02, 1.600000e+00, 0.000000e+00,
                                                      -2.180000e-01, 5.000000e-01, 4.000000e-01,
                                                      -8.170000e-02, 8.100000e-03)
                        ResamplingMethod            = Bi-Linear
                        TcoMosaicFileName           = (N/A, N/A, N/A, N/A, N/A, N/A, N/A, N/A,
                                                      N/A, N/A, N/A, N/A)
                        DtmMosaicFileName           = (N/A, N/A, N/A, N/A, N/A, N/A, N/A, N/A,
                                                      N/A, N/A, N/A, N/A)
                        OverlapSelectionId          = "OVERWRITTEN BY BIGGER REV. NUMBER SCENE &
                                                      MORE ACCURATE SPK VERSION"
                        MatchingMosaic              = N/A
                        L2aDeadPixelThreshold       = 30
                        L2aSaturationThreshold      = 1023
                        DarkValidMinimum            = -5
                        RadianceSaturationThreshold = 425.971000 <W/m**2/micron/sr>
                        RefSaturationThreshold      = 0.655340 <ND>
                        RefCnvCoef                  = 0.002396 <1/(W/m**2/micron/sr)>
                        StandardGeometry            = (30.0, 0.0, 30.0)
                        PhotoCorrId                 = USGS
                        PhotoCorrCoef               = (-1.900000e-02, 2.420000e-04, -1.460000e-06,
                                                      5.400000e-02, 1.600000e+00, 0.000000e+00,
                                                      -2.180000e-01, 5.000000e-01, 4.000000e-01,
                                                      -8.170000e-02, 8.100000e-03)
                      End_Group

                      Group = BandBin
                        FilterName = BroadBand
                        Center     = 640nm
                        Width      = 420nm
                      End_Group

                      Group = Mapping
                        ProjectionName     = SimpleCylindrical
                        CenterLongitude    = 0.0
                        TargetName         = Moon
                        EquatorialRadius   = 1737400.0 <meters>
                        PolarRadius        = 1737400.0 <meters>
                        LatitudeType       = Planetocentric
                        LongitudeDirection = PositiveEast
                        LongitudeDomain    = 360
                        MinimumLatitude    = -11.99975586
                        MaximumLatitude    = -9.0
                        MinimumLongitude   = 0.0
                        MaximumLongitude   = 2.99975586
                        UpperLeftCornerX   = 3.7015 <meters>
                        UpperLeftCornerY   = -272900.4905 <meters>
                        PixelResolution    = 7.403 <meters/pixel>
                        Scale              = 4096.0 <pixels/degree>
                      End_Group
                    End_Object

                    Object = Label
                      Bytes = 65536
                    End_Object

                    Object = History
                      Name      = IsisCube
                      StartByte = 302055425
                      Bytes     = 725
                    End_Object

                    Object = OriginalLabel
                      Name      = IsisCube
                      StartByte = 302056150
                      Bytes     = 10573
                    End_Object
                    End
    )");

  QVector<QString> args = {"from=data/isisimport/kaguyaTC12bytesImage.img", "to=" + tempDir.path() + "/kaguyatc.cub" };
  UserInterface options(APP_XML, args);

  isisimport(options);

  Pvl output = Pvl(tempDir.path() + "/kaguyatc.cub");
  Pvl truth;
  PvlInput >> truth;

  PvlGroup truthDimensions = truth.findGroup("Dimensions", Isis::Plugin::Traverse);
  PvlGroup truthInstrument = truth.findGroup("Instrument", Isis::Plugin::Traverse);
  
  PvlGroup outputDimensions = output.findGroup("Dimensions", Isis::Plugin::Traverse);
  PvlGroup outputInstrument = output.findGroup("Instrument", Isis::Plugin::Traverse);


  PvlKeyword truthLines = truthDimensions.findKeyword("Lines");
  PvlKeyword truthSamples = truthDimensions.findKeyword("Samples");
  PvlKeyword truthMissionName = truthInstrument.findKeyword("MissionName");
  PvlKeyword truthSpacecraftName = truthInstrument.findKeyword("SpacecraftName");

  PvlKeyword outputLines = outputDimensions.findKeyword("Lines");
  PvlKeyword outputSamples = outputDimensions.findKeyword("Samples");
  PvlKeyword outputMissionName = outputInstrument.findKeyword("MissionName");
  PvlKeyword outputSpacecraftName = outputInstrument.findKeyword("SpacecraftName");


  ASSERT_TRUE(PvlKeyword::stringEqual(truthLines, outputLines));
  ASSERT_TRUE(PvlKeyword::stringEqual(truthSamples, outputSamples));
  ASSERT_TRUE(PvlKeyword::stringEqual(truthMissionName, outputMissionName));
  ASSERT_TRUE(PvlKeyword::stringEqual(truthSpacecraftName, outputSpacecraftName));
}

