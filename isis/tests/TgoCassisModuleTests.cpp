#include <QTemporaryDir>

#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"

#include "tgocassis2isis.h"
#include "spiceinit.h"
#include "tgocassisstitch.h"
#include "tgocassisunstitch.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString TGOCASSIS2ISIS_XML = FileName("$ISISROOT/bin/xml/tgocassis2isis.xml").expanded();
static QString SPICEINIT_XML = FileName("$ISISROOT/bin/xml/spiceinit.xml").expanded();
static QString TGOCASSISSTITCH_XML = FileName("$ISISROOT/bin/xml/tgocassisstitch.xml").expanded();
static QString TGOCASSISUNSTITCH_XML = FileName("$ISISROOT/bin/xml/tgocassisunstitch.xml").expanded();

TEST(TgoCassisModuleTests, TgoCassisStitchUnstitch) {
  QTemporaryDir prefix;
  
  // run tgocassis2isis and spiceinit on pan framelet.
  QString panFileName = prefix.path() + "/panframelet.cub";
  QVector<QString> tgocassis2isisArgs = {"from=data/tgoCassis/CAS-MCO-2016-11-26T22.50.27.381-PAN-00005-B1.xml",  
                                         "to=" + panFileName};

  UserInterface tgocassis2isisPan(TGOCASSIS2ISIS_XML, tgocassis2isisArgs);
  try {
    tgocassis2isis(tgocassis2isisPan);
  }
  catch (IException &e) {
    FAIL() << "Unable to run tgocassis2isis on pan image: " << e.what() << std::endl;
  }
  
  QVector<QString> spiceinitArgs = {"from=" + panFileName,  "ckp=t", "spkp=t"};
  UserInterface spiceinitPan(SPICEINIT_XML, spiceinitArgs);
  try {
    spiceinit(spiceinitPan);
  }
  catch (IException &e) {
    FAIL() << "Unable to run spiceinit on pan image: " << e.what() << std::endl;
  }
  
  // run tgocassis2isis and spiceinit on red framelet.
  QString redFileName = prefix.path() + "/redframelet.cub";
  tgocassis2isisArgs = {"from=data/tgoCassis/CAS-MCO-2016-11-26T22.50.27.381-RED-01005-B1.xml",  
                        "to=" + redFileName};
  UserInterface tgocassis2isisRed(TGOCASSIS2ISIS_XML, tgocassis2isisArgs);
  try {
    tgocassis2isis(tgocassis2isisRed);
  }
  catch (IException &e) {
    FAIL() << "Unable to run tgocassis2isis on pan image: " << e.what() << std::endl;
  }
  
  spiceinitArgs = {"from=" + redFileName,  "ckp=t", "spkp=t"};
  UserInterface spiceinitRed(SPICEINIT_XML, spiceinitArgs);
  try {
    spiceinit(spiceinitRed);
  }
  catch (IException &e) {
    FAIL() << "Unable to run spiceinit on pan image: " << e.what() << std::endl;
  }
  
  // run tgocassis2isis and spiceinit on blu framelet.
  QString bluFileName = prefix.path() + "/bluframelet.cub";
  tgocassis2isisArgs = {"from=data/tgoCassis/CAS-MCO-2016-11-26T22.50.27.381-BLU-03005-B1.xml",  
                        "to=" + bluFileName};
  UserInterface tgocassis2isisBlu(TGOCASSIS2ISIS_XML, tgocassis2isisArgs);
  try {
    tgocassis2isis(tgocassis2isisBlu);
  }
  catch (IException &e) {
    FAIL() << "Unable to run tgocassis2isis on blu image: " << e.what() << std::endl;
  }
  
  spiceinitArgs = {"from=" + bluFileName,  "ckp=t", "spkp=t"};
  UserInterface spiceinitBlu(SPICEINIT_XML, spiceinitArgs);
  try {
    spiceinit(spiceinitBlu);
  }
  catch (IException &e) {
    FAIL() << "Unable to run spiceinit on blu image: " << e.what() << std::endl;
  }
  
  // run tgocassis2isis and spiceinit on nir framelet.
  QString nirFileName = prefix.path() + "/nirframelet.cub";
  tgocassis2isisArgs = {"from=data/tgoCassis/CAS-MCO-2016-11-26T22.50.27.381-NIR-02005-B1.xml",  
                        "to=" + nirFileName};
  UserInterface tgocassis2isisNir(TGOCASSIS2ISIS_XML, tgocassis2isisArgs);
  try {
    tgocassis2isis(tgocassis2isisNir);
  }
  catch (IException &e) {
    FAIL() << "Unable to run tgocassis2isis on nir image: " << e.what() << std::endl;
  }
  
  spiceinitArgs = {"from=" + nirFileName,  "ckp=t", "spkp=t"};
  UserInterface spiceinitNir(SPICEINIT_XML, spiceinitArgs);
  try {
    spiceinit(spiceinitNir);
  }
  catch (IException &e) {
    FAIL() << "Unable to run spiceinit  on nir image: " << e.what() << std::endl;
  }
  
  // run stitch and unstitch on cube list
  FileList *cubeList = new FileList();
  cubeList->append(panFileName);
  cubeList->append(redFileName);
  cubeList->append(bluFileName);
  cubeList->append(nirFileName);
  
  QString cubeListFile = prefix.path() + "/cubelist.lis";
  cubeList->write(cubeListFile);
  
  QVector<QString> stitchArgs = {"fromlist=" + cubeListFile,  
                "outputprefix=" + prefix.path() + "/stitched"};
  UserInterface stitchOptions(TGOCASSISSTITCH_XML, stitchArgs);
  
  try {
    tgocassisstitch(stitchOptions);
  }
  catch (IException &e) {
    FAIL() << "Unable to run tgocassisstitch with cube list: " << e.what() << std::endl;
  }
  
  QVector<QString> unstitchArgs = {"from=" + prefix.path() + "/stitched-2016-11-26T22:50:27.381.cub",  
                  "outputprefix=" + prefix.path() + "/unstitched"};
  UserInterface unstitchOptions(TGOCASSISUNSTITCH_XML, unstitchArgs);
  
  try {
    tgocassisunstitch(unstitchOptions);
  }
  catch (IException &e) {
    FAIL() << "Unable to run tgocassisunstitch with stitched cube: " << e.what() << std::endl;
  }
  
  // Compare Pan unstitched cube to original cube
  QString panUnstitchFile = prefix.path() + "/unstitched_PAN.cub";
  Cube panUnstitchCube(panUnstitchFile);
  Pvl *panUnstitchLabel = panUnstitchCube.label();
  
  Cube panOrigCube(panFileName);
  Pvl *panOrigLabel = panOrigCube.label();

  // Dimensions group
  EXPECT_EQ(panUnstitchCube.sampleCount(), panOrigCube.sampleCount());
  EXPECT_EQ(panUnstitchCube.lineCount(), panOrigCube.lineCount());
  EXPECT_EQ(panUnstitchCube.bandCount(), panOrigCube.bandCount());

  // Instrument Group
  PvlGroup &panUnstitchInst = panUnstitchLabel->findGroup("Instrument", Pvl::Traverse);
  PvlGroup &panOrigInst = panOrigLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_EQ(panUnstitchInst["SpacecraftName"], panOrigInst["SpacecraftName"]);
  EXPECT_EQ(panUnstitchInst["InstrumentId"], panOrigInst["InstrumentId"]);
  EXPECT_EQ(panUnstitchInst["TargetName"], panOrigInst["TargetName"]);
  EXPECT_EQ(panUnstitchInst["StartTime"], panOrigInst["StartTime"]);
  EXPECT_EQ(panUnstitchInst["SpaceCraftClockStartCount"], panOrigInst["SpaceCraftClockStartCount"]);
  EXPECT_EQ(panUnstitchInst["ExposureDuration"], panOrigInst["ExposureDuration"]);
  EXPECT_EQ(panUnstitchInst["SummingMode"], panOrigInst["SummingMode"]);
  EXPECT_EQ(panUnstitchInst["Filter"], panOrigInst["Filter"]);
  
  // Archive Group
  PvlGroup &panUnstitchArch = panUnstitchLabel->findGroup("Archive", Pvl::Traverse);
  PvlGroup &panOrigArch = panOrigLabel->findGroup("Archive", Pvl::Traverse);
  EXPECT_EQ(panUnstitchArch["DataSetId"], panOrigArch["DataSetId"]);
  EXPECT_EQ(panUnstitchArch["ProductVersionId"], panOrigArch["ProductVersionId"]);
  EXPECT_EQ(panUnstitchArch["ProductCreationTime"], panOrigArch["ProductCreationTime"]);  
  EXPECT_EQ(panUnstitchArch["ScalingFactor"], panOrigArch["ScalingFactor"]);
  EXPECT_EQ(panUnstitchArch["Offset"], panOrigArch["Offset"]);
  EXPECT_EQ(panUnstitchArch["PredictMaximumExposureTime"], panOrigArch["PredictMaximumExposureTime"]);
  EXPECT_EQ(panUnstitchArch["CassisOffNadirAngle"], panOrigArch["CassisOffNadirAngle"]);
  EXPECT_EQ(panUnstitchArch["PredictedRepetitionFrequency"], panOrigArch["PredictedRepetitionFrequency"]);
  EXPECT_EQ(panUnstitchArch["GroundTrackVelocity"], panOrigArch["GroundTrackVelocity"]);
  EXPECT_EQ(panUnstitchArch["ForwardRotationAngle"], panOrigArch["ForwardRotationAngle"]);
  EXPECT_EQ(panUnstitchArch["SpiceMisalignment"], panOrigArch["SpiceMisalignment"]);
  EXPECT_EQ(panUnstitchArch["FocalLength"], panOrigArch["FocalLength"]);
  EXPECT_EQ(panUnstitchArch["FNumber"], panOrigArch["FNumber"]);
  EXPECT_EQ(panUnstitchArch["ExposureTimeCommand"], panOrigArch["ExposureTimeCommand"]);
  EXPECT_EQ(panUnstitchArch["FrameletNumber"], panOrigArch["FrameletNumber"]);
  EXPECT_EQ(panUnstitchArch["NumberOfFramelets"], panOrigArch["NumberOfFramelets"]);
  EXPECT_EQ(panUnstitchArch["ImageFrequency"], panOrigArch["ImageFrequency"]);
  EXPECT_EQ(panUnstitchArch["NumberOfWindows"], panOrigArch["NumberOfWindows"]);
  EXPECT_EQ(panUnstitchArch["UniqueIdentifier"], panOrigArch["UniqueIdentifier"]);
  EXPECT_EQ(panUnstitchArch["ExposureTimestamp"], panOrigArch["ExposureTimestamp"]);  
  EXPECT_EQ(panUnstitchArch["ExposureTimePEHK"], panOrigArch["ExposureTimePEHK"]);
  EXPECT_EQ(panUnstitchArch["PixelsPossiblySaturated"], panOrigArch["PixelsPossiblySaturated"]);
  EXPECT_EQ(panUnstitchArch["WindowCount"], panOrigArch["WindowCount"]);
  EXPECT_EQ(panUnstitchArch["Window1Binning"], panOrigArch["Window1Binning"]);
  EXPECT_EQ(panUnstitchArch["Window1StartSample"], panOrigArch["Window1StartSample"]);    
  EXPECT_EQ(panUnstitchArch["Window1EndSample"], panOrigArch["Window1EndSample"]);
  EXPECT_EQ(panUnstitchArch["Window1StartLine"], panOrigArch["Window1StartLine"]);
  EXPECT_EQ(panUnstitchArch["Window1EndLine"], panOrigArch["Window1EndLine"]);
  EXPECT_EQ(panUnstitchArch["Window2Binning"], panOrigArch["Window2Binning"]);
  EXPECT_EQ(panUnstitchArch["Window2StartSample"], panOrigArch["Window2StartSample"]);
  EXPECT_EQ(panUnstitchArch["Window2EndSample"], panOrigArch["Window2EndSample"]);
  EXPECT_EQ(panUnstitchArch["Window2StartLine"], panOrigArch["Window2StartLine"]);
  EXPECT_EQ(panUnstitchArch["Window2EndLine"], panOrigArch["Window2EndLine"]);
  EXPECT_EQ(panUnstitchArch["Window3Binning"], panOrigArch["Window3Binning"]);
  EXPECT_EQ(panUnstitchArch["Window3StartSample"], panOrigArch["Window3StartSample"]);
  EXPECT_EQ(panUnstitchArch["Window3EndSample"], panOrigArch["Window3EndSample"]);
  EXPECT_EQ(panUnstitchArch["Window3StartLine"], panOrigArch["Window3StartLine"]);
  EXPECT_EQ(panUnstitchArch["Window3EndLine"], panOrigArch["Window3EndLine"]);  
  EXPECT_EQ(panUnstitchArch["Window4Binning"], panOrigArch["Window4Binning"]);
  EXPECT_EQ(panUnstitchArch["Window4StartSample"], panOrigArch["Window4StartSample"]);
  EXPECT_EQ(panUnstitchArch["Window4EndSample"], panOrigArch["Window4EndSample"]);
  EXPECT_EQ(panUnstitchArch["Window4StartLine"], panOrigArch["Window4StartLine"]);
  EXPECT_EQ(panUnstitchArch["Window4EndLine"], panOrigArch["Window4EndLine"]);
  EXPECT_EQ(panUnstitchArch["Window5Binning"], panOrigArch["Window5Binning"]);
  EXPECT_EQ(panUnstitchArch["Window5StartSample"], panOrigArch["Window5StartSample"]);
  EXPECT_EQ(panUnstitchArch["Window5EndSample"], panOrigArch["Window5EndSample"]);
  EXPECT_EQ(panUnstitchArch["Window5StartLine"], panOrigArch["Window5StartLine"]);
  EXPECT_EQ(panUnstitchArch["Window5EndLine"], panOrigArch["Window5EndLine"]);
  EXPECT_EQ(panUnstitchArch["Window6Binning"], panOrigArch["Window6Binning"]);
  EXPECT_EQ(panUnstitchArch["Window6StartSample"], panOrigArch["Window6StartSample"]);
  EXPECT_EQ(panUnstitchArch["Window6EndSample"], panOrigArch["Window6EndSample"]);
  EXPECT_EQ(panUnstitchArch["Window6StartLine"], panOrigArch["Window6StartLine"]);
  EXPECT_EQ(panUnstitchArch["Window6EndLine"], panOrigArch["Window6EndLine"]);
  EXPECT_EQ(panUnstitchArch["YearDoy"], panOrigArch["YearDoy"]);

  // Bandbin Group
  PvlGroup &panUnstitchBand = panUnstitchLabel->findGroup("BandBin", Pvl::Traverse);
  PvlGroup &panOrigBand = panOrigLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_EQ(panUnstitchBand["FilterName"], panOrigBand["FilterName"]);
  EXPECT_EQ(panUnstitchBand["Center"], panOrigBand["Center"]);
  EXPECT_EQ(panUnstitchBand["Width"], panOrigBand["Width"]);


  std::unique_ptr<Histogram> panUnstitchHist (panUnstitchCube.histogram());
  std::unique_ptr<Histogram> panOrigHist (panOrigCube.histogram());

  EXPECT_NEAR(panUnstitchHist->Average(), panOrigHist->Average(), 0.0001);
  EXPECT_EQ(panUnstitchHist->Sum(), panOrigHist->Sum());
  EXPECT_EQ(panUnstitchHist->ValidPixels(), panOrigHist->ValidPixels());
  EXPECT_EQ(panUnstitchHist->StandardDeviation(), panOrigHist->StandardDeviation());
  
  // Compare Red unstitched cube to original cube
  QString redUnstitchFile = prefix.path() + "/unstitched_RED.cub";
  Cube redUnstitchCube(redUnstitchFile);
  Pvl *redUnstitchLabel = redUnstitchCube.label();
  
  Cube redOrigCube(redFileName);
  Pvl *redOrigLabel = redOrigCube.label();

  // Dimensions group
  EXPECT_EQ(redUnstitchCube.sampleCount(), redOrigCube.sampleCount());
  EXPECT_EQ(redUnstitchCube.lineCount(), redOrigCube.lineCount());
  EXPECT_EQ(redUnstitchCube.bandCount(), redOrigCube.bandCount());

  // Instrument Group
  PvlGroup &redUnstitchInst = redUnstitchLabel->findGroup("Instrument", Pvl::Traverse);
  PvlGroup &redOrigInst = redOrigLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_EQ(redUnstitchInst["SpacecraftName"], redOrigInst["SpacecraftName"]);
  EXPECT_EQ(redUnstitchInst["InstrumentId"], redOrigInst["InstrumentId"]);
  EXPECT_EQ(redUnstitchInst["TargetName"], redOrigInst["TargetName"]);
  EXPECT_EQ(redUnstitchInst["StartTime"], redOrigInst["StartTime"]);
  EXPECT_EQ(redUnstitchInst["SpaceCraftClockStartCount"], redOrigInst["SpaceCraftClockStartCount"]);
  EXPECT_EQ(redUnstitchInst["ExposureDuration"], redOrigInst["ExposureDuration"]);
  EXPECT_EQ(redUnstitchInst["SummingMode"], redOrigInst["SummingMode"]);
  EXPECT_EQ(redUnstitchInst["Filter"], redOrigInst["Filter"]);
  
  // Archive Group
  PvlGroup &redUnstitchArch = redUnstitchLabel->findGroup("Archive", Pvl::Traverse);
  PvlGroup &redOrigArch = redOrigLabel->findGroup("Archive", Pvl::Traverse);
  EXPECT_EQ(redUnstitchArch["DataSetId"], redOrigArch["DataSetId"]);
  EXPECT_EQ(redUnstitchArch["ProductVersionId"], redOrigArch["ProductVersionId"]);
  EXPECT_EQ(redUnstitchArch["ProductCreationTime"], redOrigArch["ProductCreationTime"]);  
  EXPECT_EQ(redUnstitchArch["ScalingFactor"], redOrigArch["ScalingFactor"]);
  EXPECT_EQ(redUnstitchArch["Offset"], redOrigArch["Offset"]);
  EXPECT_EQ(redUnstitchArch["PredictMaximumExposureTime"], redOrigArch["PredictMaximumExposureTime"]);
  EXPECT_EQ(redUnstitchArch["CassisOffNadirAngle"], redOrigArch["CassisOffNadirAngle"]);
  EXPECT_EQ(redUnstitchArch["PredictedRepetitionFrequency"], redOrigArch["PredictedRepetitionFrequency"]);
  EXPECT_EQ(redUnstitchArch["GroundTrackVelocity"], redOrigArch["GroundTrackVelocity"]);
  EXPECT_EQ(redUnstitchArch["ForwardRotationAngle"], redOrigArch["ForwardRotationAngle"]);
  EXPECT_EQ(redUnstitchArch["SpiceMisalignment"], redOrigArch["SpiceMisalignment"]);
  EXPECT_EQ(redUnstitchArch["FocalLength"], redOrigArch["FocalLength"]);
  EXPECT_EQ(redUnstitchArch["FNumber"], redOrigArch["FNumber"]);
  EXPECT_EQ(redUnstitchArch["ExposureTimeCommand"], redOrigArch["ExposureTimeCommand"]);
  EXPECT_EQ(redUnstitchArch["FrameletNumber"], redOrigArch["FrameletNumber"]);
  EXPECT_EQ(redUnstitchArch["NumberOfFramelets"], redOrigArch["NumberOfFramelets"]);
  EXPECT_EQ(redUnstitchArch["ImageFrequency"], redOrigArch["ImageFrequency"]);
  EXPECT_EQ(redUnstitchArch["NumberOfWindows"], redOrigArch["NumberOfWindows"]);
  EXPECT_EQ(redUnstitchArch["UniqueIdentifier"], redOrigArch["UniqueIdentifier"]);
  EXPECT_EQ(redUnstitchArch["ExposureTimestamp"], redOrigArch["ExposureTimestamp"]);  
  EXPECT_EQ(redUnstitchArch["ExposureTimePEHK"], redOrigArch["ExposureTimePEHK"]);
  EXPECT_EQ(redUnstitchArch["PixelsPossiblySaturated"], redOrigArch["PixelsPossiblySaturated"]);
  EXPECT_EQ(redUnstitchArch["WindowCount"], redOrigArch["WindowCount"]);
  EXPECT_EQ(redUnstitchArch["Window1Binning"], redOrigArch["Window1Binning"]);
  EXPECT_EQ(redUnstitchArch["Window1StartSample"], redOrigArch["Window1StartSample"]);    
  EXPECT_EQ(redUnstitchArch["Window1EndSample"], redOrigArch["Window1EndSample"]);
  EXPECT_EQ(redUnstitchArch["Window1StartLine"], redOrigArch["Window1StartLine"]);
  EXPECT_EQ(redUnstitchArch["Window1EndLine"], redOrigArch["Window1EndLine"]);
  EXPECT_EQ(redUnstitchArch["Window2Binning"], redOrigArch["Window2Binning"]);
  EXPECT_EQ(redUnstitchArch["Window2StartSample"], redOrigArch["Window2StartSample"]);
  EXPECT_EQ(redUnstitchArch["Window2EndSample"], redOrigArch["Window2EndSample"]);
  EXPECT_EQ(redUnstitchArch["Window2StartLine"], redOrigArch["Window2StartLine"]);
  EXPECT_EQ(redUnstitchArch["Window2EndLine"], redOrigArch["Window2EndLine"]);
  EXPECT_EQ(redUnstitchArch["Window3Binning"], redOrigArch["Window3Binning"]);
  EXPECT_EQ(redUnstitchArch["Window3StartSample"], redOrigArch["Window3StartSample"]);
  EXPECT_EQ(redUnstitchArch["Window3EndSample"], redOrigArch["Window3EndSample"]);
  EXPECT_EQ(redUnstitchArch["Window3StartLine"], redOrigArch["Window3StartLine"]);
  EXPECT_EQ(redUnstitchArch["Window3EndLine"], redOrigArch["Window3EndLine"]);  
  EXPECT_EQ(redUnstitchArch["Window4Binning"], redOrigArch["Window4Binning"]);
  EXPECT_EQ(redUnstitchArch["Window4StartSample"], redOrigArch["Window4StartSample"]);
  EXPECT_EQ(redUnstitchArch["Window4EndSample"], redOrigArch["Window4EndSample"]);
  EXPECT_EQ(redUnstitchArch["Window4StartLine"], redOrigArch["Window4StartLine"]);
  EXPECT_EQ(redUnstitchArch["Window4EndLine"], redOrigArch["Window4EndLine"]);
  EXPECT_EQ(redUnstitchArch["Window5Binning"], redOrigArch["Window5Binning"]);
  EXPECT_EQ(redUnstitchArch["Window5StartSample"], redOrigArch["Window5StartSample"]);
  EXPECT_EQ(redUnstitchArch["Window5EndSample"], redOrigArch["Window5EndSample"]);
  EXPECT_EQ(redUnstitchArch["Window5StartLine"], redOrigArch["Window5StartLine"]);
  EXPECT_EQ(redUnstitchArch["Window5EndLine"], redOrigArch["Window5EndLine"]);
  EXPECT_EQ(redUnstitchArch["Window6Binning"], redOrigArch["Window6Binning"]);
  EXPECT_EQ(redUnstitchArch["Window6StartSample"], redOrigArch["Window6StartSample"]);
  EXPECT_EQ(redUnstitchArch["Window6EndSample"], redOrigArch["Window6EndSample"]);
  EXPECT_EQ(redUnstitchArch["Window6StartLine"], redOrigArch["Window6StartLine"]);
  EXPECT_EQ(redUnstitchArch["Window6EndLine"], redOrigArch["Window6EndLine"]);
  EXPECT_EQ(redUnstitchArch["YearDoy"], redOrigArch["YearDoy"]);

  // Bandbin Group
  PvlGroup &redUnstitchBand = redUnstitchLabel->findGroup("BandBin", Pvl::Traverse);
  PvlGroup &redOrigBand = redOrigLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_EQ(redUnstitchBand["FilterName"], redOrigBand["FilterName"]);
  EXPECT_EQ(redUnstitchBand["Center"], redOrigBand["Center"]);
  EXPECT_EQ(redUnstitchBand["Width"], redOrigBand["Width"]);


  std::unique_ptr<Histogram> redUnstitchHist (redUnstitchCube.histogram());
  std::unique_ptr<Histogram> redOrigHist (redOrigCube.histogram());

  EXPECT_NEAR(redUnstitchHist->Average(), redOrigHist->Average(), 0.0001);
  EXPECT_EQ(redUnstitchHist->Sum(), redOrigHist->Sum());
  EXPECT_EQ(redUnstitchHist->ValidPixels(), redOrigHist->ValidPixels());
  EXPECT_EQ(redUnstitchHist->StandardDeviation(), redOrigHist->StandardDeviation());
  
  
  // compare Blu unstitched cube to original cube
  QString bluUnstitchFile = prefix.path() + "/unstitched_BLU.cub";
  Cube bluUnstitchCube(bluUnstitchFile);
  Pvl *bluUnstitchLabel = bluUnstitchCube.label();
  
  Cube bluOrigCube(bluFileName);
  Pvl *bluOrigLabel = bluOrigCube.label();

  // Dimensions group
  EXPECT_EQ(bluUnstitchCube.sampleCount(), bluOrigCube.sampleCount());
  EXPECT_EQ(bluUnstitchCube.lineCount(), bluOrigCube.lineCount());
  EXPECT_EQ(bluUnstitchCube.bandCount(), bluOrigCube.bandCount());

  // Instrument Group
  PvlGroup &bluUnstitchInst = bluUnstitchLabel->findGroup("Instrument", Pvl::Traverse);
  PvlGroup &bluOrigInst = bluOrigLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_EQ(bluUnstitchInst["SpacecraftName"], bluOrigInst["SpacecraftName"]);
  EXPECT_EQ(bluUnstitchInst["InstrumentId"], bluOrigInst["InstrumentId"]);
  EXPECT_EQ(bluUnstitchInst["TargetName"], bluOrigInst["TargetName"]);
  EXPECT_EQ(bluUnstitchInst["StartTime"], bluOrigInst["StartTime"]);
  EXPECT_EQ(bluUnstitchInst["SpaceCraftClockStartCount"], bluOrigInst["SpaceCraftClockStartCount"]);
  EXPECT_EQ(bluUnstitchInst["ExposureDuration"], bluOrigInst["ExposureDuration"]);
  EXPECT_EQ(bluUnstitchInst["SummingMode"], bluOrigInst["SummingMode"]);
  EXPECT_EQ(bluUnstitchInst["Filter"], bluOrigInst["Filter"]);
  
  // Archive Group
  PvlGroup &bluUnstitchArch = bluUnstitchLabel->findGroup("Archive", Pvl::Traverse);
  PvlGroup &bluOrigArch = bluOrigLabel->findGroup("Archive", Pvl::Traverse);
  EXPECT_EQ(bluUnstitchArch["DataSetId"], bluOrigArch["DataSetId"]);
  EXPECT_EQ(bluUnstitchArch["ProductVersionId"], bluOrigArch["ProductVersionId"]);
  EXPECT_EQ(bluUnstitchArch["ProductCreationTime"], bluOrigArch["ProductCreationTime"]);  
  EXPECT_EQ(bluUnstitchArch["ScalingFactor"], bluOrigArch["ScalingFactor"]);
  EXPECT_EQ(bluUnstitchArch["Offset"], bluOrigArch["Offset"]);
  EXPECT_EQ(bluUnstitchArch["PredictMaximumExposureTime"], bluOrigArch["PredictMaximumExposureTime"]);
  EXPECT_EQ(bluUnstitchArch["CassisOffNadirAngle"], bluOrigArch["CassisOffNadirAngle"]);
  EXPECT_EQ(bluUnstitchArch["PredictedRepetitionFrequency"], bluOrigArch["PredictedRepetitionFrequency"]);
  EXPECT_EQ(bluUnstitchArch["GroundTrackVelocity"], bluOrigArch["GroundTrackVelocity"]);
  EXPECT_EQ(bluUnstitchArch["ForwardRotationAngle"], bluOrigArch["ForwardRotationAngle"]);
  EXPECT_EQ(bluUnstitchArch["SpiceMisalignment"], bluOrigArch["SpiceMisalignment"]);
  EXPECT_EQ(bluUnstitchArch["FocalLength"], bluOrigArch["FocalLength"]);
  EXPECT_EQ(bluUnstitchArch["FNumber"], bluOrigArch["FNumber"]);
  EXPECT_EQ(bluUnstitchArch["ExposureTimeCommand"], bluOrigArch["ExposureTimeCommand"]);
  EXPECT_EQ(bluUnstitchArch["FrameletNumber"], bluOrigArch["FrameletNumber"]);
  EXPECT_EQ(bluUnstitchArch["NumberOfFramelets"], bluOrigArch["NumberOfFramelets"]);
  EXPECT_EQ(bluUnstitchArch["ImageFrequency"], bluOrigArch["ImageFrequency"]);
  EXPECT_EQ(bluUnstitchArch["NumberOfWindows"], bluOrigArch["NumberOfWindows"]);
  EXPECT_EQ(bluUnstitchArch["UniqueIdentifier"], bluOrigArch["UniqueIdentifier"]);
  EXPECT_EQ(bluUnstitchArch["ExposureTimestamp"], bluOrigArch["ExposureTimestamp"]);  
  EXPECT_EQ(bluUnstitchArch["ExposureTimePEHK"], bluOrigArch["ExposureTimePEHK"]);
  EXPECT_EQ(bluUnstitchArch["PixelsPossiblySaturated"], bluOrigArch["PixelsPossiblySaturated"]);
  EXPECT_EQ(bluUnstitchArch["WindowCount"], bluOrigArch["WindowCount"]);
  EXPECT_EQ(bluUnstitchArch["Window1Binning"], bluOrigArch["Window1Binning"]);
  EXPECT_EQ(bluUnstitchArch["Window1StartSample"], bluOrigArch["Window1StartSample"]);    
  EXPECT_EQ(bluUnstitchArch["Window1EndSample"], bluOrigArch["Window1EndSample"]);
  EXPECT_EQ(bluUnstitchArch["Window1StartLine"], bluOrigArch["Window1StartLine"]);
  EXPECT_EQ(bluUnstitchArch["Window1EndLine"], bluOrigArch["Window1EndLine"]);
  EXPECT_EQ(bluUnstitchArch["Window2Binning"], bluOrigArch["Window2Binning"]);
  EXPECT_EQ(bluUnstitchArch["Window2StartSample"], bluOrigArch["Window2StartSample"]);
  EXPECT_EQ(bluUnstitchArch["Window2EndSample"], bluOrigArch["Window2EndSample"]);
  EXPECT_EQ(bluUnstitchArch["Window2StartLine"], bluOrigArch["Window2StartLine"]);
  EXPECT_EQ(bluUnstitchArch["Window2EndLine"], bluOrigArch["Window2EndLine"]);
  EXPECT_EQ(bluUnstitchArch["Window3Binning"], bluOrigArch["Window3Binning"]);
  EXPECT_EQ(bluUnstitchArch["Window3StartSample"], bluOrigArch["Window3StartSample"]);
  EXPECT_EQ(bluUnstitchArch["Window3EndSample"], bluOrigArch["Window3EndSample"]);
  EXPECT_EQ(bluUnstitchArch["Window3StartLine"], bluOrigArch["Window3StartLine"]);
  EXPECT_EQ(bluUnstitchArch["Window3EndLine"], bluOrigArch["Window3EndLine"]);  
  EXPECT_EQ(bluUnstitchArch["Window4Binning"], bluOrigArch["Window4Binning"]);
  EXPECT_EQ(bluUnstitchArch["Window4StartSample"], bluOrigArch["Window4StartSample"]);
  EXPECT_EQ(bluUnstitchArch["Window4EndSample"], bluOrigArch["Window4EndSample"]);
  EXPECT_EQ(bluUnstitchArch["Window4StartLine"], bluOrigArch["Window4StartLine"]);
  EXPECT_EQ(bluUnstitchArch["Window4EndLine"], bluOrigArch["Window4EndLine"]);
  EXPECT_EQ(bluUnstitchArch["Window5Binning"], bluOrigArch["Window5Binning"]);
  EXPECT_EQ(bluUnstitchArch["Window5StartSample"], bluOrigArch["Window5StartSample"]);
  EXPECT_EQ(bluUnstitchArch["Window5EndSample"], bluOrigArch["Window5EndSample"]);
  EXPECT_EQ(bluUnstitchArch["Window5StartLine"], bluOrigArch["Window5StartLine"]);
  EXPECT_EQ(bluUnstitchArch["Window5EndLine"], bluOrigArch["Window5EndLine"]);
  EXPECT_EQ(bluUnstitchArch["Window6Binning"], bluOrigArch["Window6Binning"]);
  EXPECT_EQ(bluUnstitchArch["Window6StartSample"], bluOrigArch["Window6StartSample"]);
  EXPECT_EQ(bluUnstitchArch["Window6EndSample"], bluOrigArch["Window6EndSample"]);
  EXPECT_EQ(bluUnstitchArch["Window6StartLine"], bluOrigArch["Window6StartLine"]);
  EXPECT_EQ(bluUnstitchArch["Window6EndLine"], bluOrigArch["Window6EndLine"]);
  EXPECT_EQ(bluUnstitchArch["YearDoy"], bluOrigArch["YearDoy"]);

  // Bandbin Group
  PvlGroup &bluUnstitchBand = bluUnstitchLabel->findGroup("BandBin", Pvl::Traverse);
  PvlGroup &bluOrigBand = bluOrigLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_EQ(bluUnstitchBand["FilterName"], bluOrigBand["FilterName"]);
  EXPECT_EQ(bluUnstitchBand["Center"], bluOrigBand["Center"]);
  EXPECT_EQ(bluUnstitchBand["Width"], bluOrigBand["Width"]);


  std::unique_ptr<Histogram> bluUnstitchHist (bluUnstitchCube.histogram());
  std::unique_ptr<Histogram> bluOrigHist (bluOrigCube.histogram());

  EXPECT_NEAR(bluUnstitchHist->Average(), bluOrigHist->Average(), 0.0001);
  EXPECT_EQ(bluUnstitchHist->Sum(), bluOrigHist->Sum());
  EXPECT_EQ(bluUnstitchHist->ValidPixels(), bluOrigHist->ValidPixels());
  EXPECT_EQ(bluUnstitchHist->StandardDeviation(), bluOrigHist->StandardDeviation());
  
  
  // compare Nir unstitched cube to original cube
  QString nirUnstitchFile = prefix.path() + "/unstitched_NIR.cub";
  Cube nirUnstitchCube(nirUnstitchFile);
  Pvl *nirUnstitchLabel = nirUnstitchCube.label();
  
  Cube nirOrigCube(nirFileName);
  Pvl *nirOrigLabel = nirOrigCube.label();

  // Dimensions group
  EXPECT_EQ(nirUnstitchCube.sampleCount(), nirOrigCube.sampleCount());
  EXPECT_EQ(nirUnstitchCube.lineCount(), nirOrigCube.lineCount());
  EXPECT_EQ(nirUnstitchCube.bandCount(), nirOrigCube.bandCount());

  // Instrument Group
  PvlGroup &nirUnstitchInst = nirUnstitchLabel->findGroup("Instrument", Pvl::Traverse);
  PvlGroup &nirOrigInst = nirOrigLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_EQ(nirUnstitchInst["SpacecraftName"], nirOrigInst["SpacecraftName"]);
  EXPECT_EQ(nirUnstitchInst["InstrumentId"], nirOrigInst["InstrumentId"]);
  EXPECT_EQ(nirUnstitchInst["TargetName"], nirOrigInst["TargetName"]);
  EXPECT_EQ(nirUnstitchInst["StartTime"], nirOrigInst["StartTime"]);
  EXPECT_EQ(nirUnstitchInst["SpaceCraftClockStartCount"], nirOrigInst["SpaceCraftClockStartCount"]);
  EXPECT_EQ(nirUnstitchInst["ExposureDuration"], nirOrigInst["ExposureDuration"]);
  EXPECT_EQ(nirUnstitchInst["SummingMode"], nirOrigInst["SummingMode"]);
  EXPECT_EQ(nirUnstitchInst["Filter"], nirOrigInst["Filter"]);
  
  // Archive Group
  PvlGroup &nirUnstitchArch = nirUnstitchLabel->findGroup("Archive", Pvl::Traverse);
  PvlGroup &nirOrigArch = nirOrigLabel->findGroup("Archive", Pvl::Traverse);
  EXPECT_EQ(nirUnstitchArch["DataSetId"], nirOrigArch["DataSetId"]);
  EXPECT_EQ(nirUnstitchArch["ProductVersionId"], nirOrigArch["ProductVersionId"]);
  EXPECT_EQ(nirUnstitchArch["ProductCreationTime"], nirOrigArch["ProductCreationTime"]);  
  EXPECT_EQ(nirUnstitchArch["ScalingFactor"], nirOrigArch["ScalingFactor"]);
  EXPECT_EQ(nirUnstitchArch["Offset"], nirOrigArch["Offset"]);
  EXPECT_EQ(nirUnstitchArch["PredictMaximumExposureTime"], nirOrigArch["PredictMaximumExposureTime"]);
  EXPECT_EQ(nirUnstitchArch["CassisOffNadirAngle"], nirOrigArch["CassisOffNadirAngle"]);
  EXPECT_EQ(nirUnstitchArch["PredictedRepetitionFrequency"], nirOrigArch["PredictedRepetitionFrequency"]);
  EXPECT_EQ(nirUnstitchArch["GroundTrackVelocity"], nirOrigArch["GroundTrackVelocity"]);
  EXPECT_EQ(nirUnstitchArch["ForwardRotationAngle"], nirOrigArch["ForwardRotationAngle"]);
  EXPECT_EQ(nirUnstitchArch["SpiceMisalignment"], nirOrigArch["SpiceMisalignment"]);
  EXPECT_EQ(nirUnstitchArch["FocalLength"], nirOrigArch["FocalLength"]);
  EXPECT_EQ(nirUnstitchArch["FNumber"], nirOrigArch["FNumber"]);
  EXPECT_EQ(nirUnstitchArch["ExposureTimeCommand"], nirOrigArch["ExposureTimeCommand"]);
  EXPECT_EQ(nirUnstitchArch["FrameletNumber"], nirOrigArch["FrameletNumber"]);
  EXPECT_EQ(nirUnstitchArch["NumberOfFramelets"], nirOrigArch["NumberOfFramelets"]);
  EXPECT_EQ(nirUnstitchArch["ImageFrequency"], nirOrigArch["ImageFrequency"]);
  EXPECT_EQ(nirUnstitchArch["NumberOfWindows"], nirOrigArch["NumberOfWindows"]);
  EXPECT_EQ(nirUnstitchArch["UniqueIdentifier"], nirOrigArch["UniqueIdentifier"]);
  EXPECT_EQ(nirUnstitchArch["ExposureTimestamp"], nirOrigArch["ExposureTimestamp"]);  
  EXPECT_EQ(nirUnstitchArch["ExposureTimePEHK"], nirOrigArch["ExposureTimePEHK"]);
  EXPECT_EQ(nirUnstitchArch["PixelsPossiblySaturated"], nirOrigArch["PixelsPossiblySaturated"]);
  EXPECT_EQ(nirUnstitchArch["WindowCount"], nirOrigArch["WindowCount"]);
  EXPECT_EQ(nirUnstitchArch["Window1Binning"], nirOrigArch["Window1Binning"]);
  EXPECT_EQ(nirUnstitchArch["Window1StartSample"], nirOrigArch["Window1StartSample"]);    
  EXPECT_EQ(nirUnstitchArch["Window1EndSample"], nirOrigArch["Window1EndSample"]);
  EXPECT_EQ(nirUnstitchArch["Window1StartLine"], nirOrigArch["Window1StartLine"]);
  EXPECT_EQ(nirUnstitchArch["Window1EndLine"], nirOrigArch["Window1EndLine"]);
  EXPECT_EQ(nirUnstitchArch["Window2Binning"], nirOrigArch["Window2Binning"]);
  EXPECT_EQ(nirUnstitchArch["Window2StartSample"], nirOrigArch["Window2StartSample"]);
  EXPECT_EQ(nirUnstitchArch["Window2EndSample"], nirOrigArch["Window2EndSample"]);
  EXPECT_EQ(nirUnstitchArch["Window2StartLine"], nirOrigArch["Window2StartLine"]);
  EXPECT_EQ(nirUnstitchArch["Window2EndLine"], nirOrigArch["Window2EndLine"]);
  EXPECT_EQ(nirUnstitchArch["Window3Binning"], nirOrigArch["Window3Binning"]);
  EXPECT_EQ(nirUnstitchArch["Window3StartSample"], nirOrigArch["Window3StartSample"]);
  EXPECT_EQ(nirUnstitchArch["Window3EndSample"], nirOrigArch["Window3EndSample"]);
  EXPECT_EQ(nirUnstitchArch["Window3StartLine"], nirOrigArch["Window3StartLine"]);
  EXPECT_EQ(nirUnstitchArch["Window3EndLine"], nirOrigArch["Window3EndLine"]);  
  EXPECT_EQ(nirUnstitchArch["Window4Binning"], nirOrigArch["Window4Binning"]);
  EXPECT_EQ(nirUnstitchArch["Window4StartSample"], nirOrigArch["Window4StartSample"]);
  EXPECT_EQ(nirUnstitchArch["Window4EndSample"], nirOrigArch["Window4EndSample"]);
  EXPECT_EQ(nirUnstitchArch["Window4StartLine"], nirOrigArch["Window4StartLine"]);
  EXPECT_EQ(nirUnstitchArch["Window4EndLine"], nirOrigArch["Window4EndLine"]);
  EXPECT_EQ(nirUnstitchArch["Window5Binning"], nirOrigArch["Window5Binning"]);
  EXPECT_EQ(nirUnstitchArch["Window5StartSample"], nirOrigArch["Window5StartSample"]);
  EXPECT_EQ(nirUnstitchArch["Window5EndSample"], nirOrigArch["Window5EndSample"]);
  EXPECT_EQ(nirUnstitchArch["Window5StartLine"], nirOrigArch["Window5StartLine"]);
  EXPECT_EQ(nirUnstitchArch["Window5EndLine"], nirOrigArch["Window5EndLine"]);
  EXPECT_EQ(nirUnstitchArch["Window6Binning"], nirOrigArch["Window6Binning"]);
  EXPECT_EQ(nirUnstitchArch["Window6StartSample"], nirOrigArch["Window6StartSample"]);
  EXPECT_EQ(nirUnstitchArch["Window6EndSample"], nirOrigArch["Window6EndSample"]);
  EXPECT_EQ(nirUnstitchArch["Window6StartLine"], nirOrigArch["Window6StartLine"]);
  EXPECT_EQ(nirUnstitchArch["Window6EndLine"], nirOrigArch["Window6EndLine"]);
  EXPECT_EQ(nirUnstitchArch["YearDoy"], nirOrigArch["YearDoy"]);

  // Bandbin Group
  PvlGroup &nirUnstitchBand = nirUnstitchLabel->findGroup("BandBin", Pvl::Traverse);
  PvlGroup &nirOrigBand = nirOrigLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_EQ(nirUnstitchBand["FilterName"], nirOrigBand["FilterName"]);
  EXPECT_EQ(nirUnstitchBand["Center"], nirOrigBand["Center"]);
  EXPECT_EQ(nirUnstitchBand["Width"], nirOrigBand["Width"]);


  std::unique_ptr<Histogram> nirUnstitchHist (nirUnstitchCube.histogram());
  std::unique_ptr<Histogram> nirOrigHist (nirOrigCube.histogram());

  EXPECT_NEAR(nirUnstitchHist->Average(), nirOrigHist->Average(), 0.0001);
  EXPECT_EQ(nirUnstitchHist->Sum(), nirOrigHist->Sum());
  EXPECT_EQ(nirUnstitchHist->ValidPixels(), nirOrigHist->ValidPixels());
  EXPECT_EQ(nirUnstitchHist->StandardDeviation(), nirOrigHist->StandardDeviation());
}