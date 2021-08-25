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