#include <QTemporaryDir>

#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

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
  QVector<QString> tgocassis2isisArgs = {"from=data/tgoCassis/stitchUnstitch/CAS-MCO-2016-11-26T22.50.27.381-PAN-00005-B1.xml",  
                                         "to=" + panFileName};

  UserInterface tgocassis2isisPan(TGOCASSIS2ISIS_XML, tgocassis2isisArgs);
  try {
    tgocassis2isis(tgocassis2isisPan);
  }
  catch (IException &e) {
    FAIL() << "Unable to run tgocassis2isis on image: " << e.what() << std::endl;
  }
  
  QVector<QString> spiceinitArgs = {"from=" + panFileName,  "ckp=t", "spkp=t"};
  UserInterface spiceinitPan(SPICEINIT_XML, spiceinitArgs);
  try {
    spiceinit(spiceinitPan);
  }
  catch (IException &e) {
    FAIL() << "Unable to spiceinit image: " << e.what() << std::endl;
  }
  
  // run tgocassis2isis and spiceinit on red framelet.
  QString redFileName = prefix.path() + "/redframelet.cub";
  tgocassis2isisArgs = {"from=data/tgoCassis/stitchUnstitch/CAS-MCO-2016-11-26T22.50.27.381-RED-01005-B1.xml",  
                        "to=" + redFileName};
  UserInterface tgocassis2isisRed(TGOCASSIS2ISIS_XML, tgocassis2isisArgs);
  try {
    tgocassis2isis(tgocassis2isisRed);
  }
  catch (IException &e) {
    FAIL() << "Unable to run tgocassis2isis on image: " << e.what() << std::endl;
  }
  
  spiceinitArgs = {"from=" + redFileName,  "ckp=t", "spkp=t"};
  UserInterface spiceinitRed(SPICEINIT_XML, spiceinitArgs);
  try {
    spiceinit(spiceinitRed);
  }
  catch (IException &e) {
    FAIL() << "Unable to spiceinit image: " << e.what() << std::endl;
  }
  
  // run tgocassis2isis and spiceinit on blu framelet.
  QString bluFileName = prefix.path() + "/bluframelet.cub";
  tgocassis2isisArgs = {"from=data/tgoCassis/stitchUnstitch/CAS-MCO-2016-11-26T22.50.27.381-BLU-03005-B1.xml",  
                        "to=" + bluFileName};
  UserInterface tgocassis2isisBlu(TGOCASSIS2ISIS_XML, tgocassis2isisArgs);
  try {
    tgocassis2isis(tgocassis2isisBlu);
  }
  catch (IException &e) {
    FAIL() << "Unable to run tgocassis2isis on image: " << e.what() << std::endl;
  }
  
  spiceinitArgs = {"from=" + bluFileName,  "ckp=t", "spkp=t"};
  UserInterface spiceinitBlu(SPICEINIT_XML, spiceinitArgs);
  try {
    spiceinit(spiceinitBlu);
  }
  catch (IException &e) {
    FAIL() << "Unable to spiceinit image: " << e.what() << std::endl;
  }
  
  // run tgocassis2isis and spiceinit on nir framelet.
  QString nirFileName = prefix.path() + "/nirframelet.cub";
  tgocassis2isisArgs = {"from=data/tgoCassis/stitchUnstitch/CAS-MCO-2016-11-26T22.50.27.381-NIR-02005-B1.xml",  
                        "to=" + nirFileName};
  UserInterface tgocassis2isisNir(TGOCASSIS2ISIS_XML, tgocassis2isisArgs);
  try {
    tgocassis2isis(tgocassis2isisNir);
  }
  catch (IException &e) {
    FAIL() << "Unable to run tgocassis2isis on image: " << e.what() << std::endl;
  }
  
  spiceinitArgs = {"from=" + nirFileName,  "ckp=t", "spkp=t"};
  UserInterface spiceinitNir(SPICEINIT_XML, spiceinitArgs);
  try {
    spiceinit(spiceinitNir);
  }
  catch (IException &e) {
    FAIL() << "Unable to spiceinit image: " << e.what() << std::endl;
  }
}