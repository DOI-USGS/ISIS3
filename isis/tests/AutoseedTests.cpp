#include <iostream>
#include <QTemporaryFile>
#include <QTemporaryDir>

#include "autoseed.h"
#include "findimageoverlaps.h"
#include "footprintinit.h"

#include "Cube.h"
#include "CubeAttribute.h"
#include "FileName.h"
#include "ImageOverlapSet.h"
#include "PixelType.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "SerialNumberList.h"
#include "TestUtilities.h"

#include "Fixtures.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString FOOTPRINT_XML = FileName("$ISISROOT/bin/xml/footprintinit.xml").expanded();
static QString OVERLAP_XML = FileName("$ISISROOT/bin/xml/findimageoverlaps.xml").expanded();
static QString AUTOSEED_XML = FileName("$ISISROOT/bin/xml/autoseed.xml").expanded();

TEST_F(ThreeImageNetwork, FunctionalTestAutoseedDefault) {
  QTemporaryDir prefix;
  ASSERT_TRUE(prefix.isValid());

  QVector<QString> footprintArgs = {};
  UserInterface footprintUi(FOOTPRINT_XML, footprintArgs);
  footprintinit(cube1, footprintUi);
  cube1->reopen();
  footprintinit(cube2, footprintUi);
  cube2->reopen();
  footprintinit(cube3, footprintUi);
  cube3->reopen();

  Pvl *log = NULL;
  QString defFile = prefix.path()+"/gridPixels.pvl";
  QString outnet = prefix.path()+"/seeded.net";

  PvlObject autoseedObject("AutoSeed");
  PvlGroup autoseedGroup("PolygonSeederAlgorithm");
  autoseedGroup.addKeyword(PvlKeyword("Name", "Grid"));
  autoseedGroup.addKeyword(PvlKeyword("MinimumThickness", "0.0"));
  autoseedGroup.addKeyword(PvlKeyword("MinimumArea", "1000"));
  autoseedGroup.addKeyword(PvlKeyword("XSpacing", "24000"));
  autoseedGroup.addKeyword(PvlKeyword("YSpacing", "24000"));
  autoseedGroup.addKeyword(PvlKeyword("PixelsFromEdge", "20.0"));
  autoseedObject.addGroup(autoseedGroup);

  Pvl autoseedDef;
  autoseedDef.addObject(autoseedObject);
  autoseedDef.write(defFile);

  QVector<QString> autoseedArgs = {"fromlist="+cubeListTempPath.fileName(),
                                    "onet="+outnet,
                                    "deffile="+defFile,
                                    "overlaplist="+imageOverlapFile->original(),
                                    "networkid=1",
                                    "pointid=??",
                                    "description=autoseed test network"};
  UserInterface autoseedUi(AUTOSEED_XML, autoseedArgs);

  autoseed(autoseedUi, log);
  ControlNet onet(outnet);
  ASSERT_EQ(onet.GetNumPoints(), 26);
}

TEST_F(ThreeImageNetwork, FunctionalTestAutoseedDeffiles) {
  QTemporaryDir prefix;
  ASSERT_TRUE(prefix.isValid());

  QVector<QString> footprintArgs = {};
  UserInterface footprintUi(FOOTPRINT_XML, footprintArgs);
  footprintinit(cube1, footprintUi);
  cube1->reopen();
  footprintinit(cube2, footprintUi);
  cube2->reopen();
  footprintinit(cube3, footprintUi);
  cube3->reopen();

  QTemporaryFile overlapList;
  overlapList.open();
  QVector<QString> overlapArgs = {"fromlist="+cubeListTempPath.fileName(),
                                  "overlaplist="+overlapList.fileName()};
  UserInterface overlapUi(OVERLAP_XML, overlapArgs);
  findimageoverlaps(overlapUi);

  Pvl *log = NULL;
  QString defFile = prefix.path()+"/pixelGrid.pvl";
  QString outnet = prefix.path()+"/new.net";

  QVector<QString> autoseedArgs = {"fromlist="+cubeListTempPath.fileName(),
                                    "onet="+outnet,
                                    "deffile="+defFile,
                                    "overlaplist="+overlapList.fileName(),
                                    "networkid=1",
                                    "pointid=???",
                                    "description=autoseed test network"};
  UserInterface autoseedUi(AUTOSEED_XML, autoseedArgs);

  // Grid DN
  PvlObject autoseedObject("AutoSeed");
  PvlGroup autoseedGroup("PolygonSeederAlgorithm");
  autoseedGroup.addKeyword(PvlKeyword("Name", "Grid"));
  autoseedGroup.addKeyword(PvlKeyword("MinimumThickness", "0.0"));
  autoseedGroup.addKeyword(PvlKeyword("MinimumArea", "1000"));
  autoseedGroup.addKeyword(PvlKeyword("XSpacing", "20000"));
  autoseedGroup.addKeyword(PvlKeyword("YSpacing", "10000"));
  autoseedGroup.addKeyword(PvlKeyword("MinDN", "0.145"));
  autoseedGroup.addKeyword(PvlKeyword("MaxDN", "0.175"));
  autoseedObject.addGroup(autoseedGroup);

  Pvl autoseedDef;
  autoseedDef.addObject(autoseedObject);
  autoseedDef.write(defFile);

  autoseed(autoseedUi, log);
  ControlNet onet(outnet);
  ASSERT_EQ(onet.GetNumValidPoints(), 0);
  ASSERT_EQ(onet.GetNumValidMeasures(), 0);

  // Grid Emission
  autoseedDef.findObject("AutoSeed").findGroup("PolygonSeederAlgorithm").deleteKeyword("MinDN");
  autoseedDef.findObject("AutoSeed").findGroup("PolygonSeederAlgorithm").deleteKeyword("MaxDN");
  autoseedDef.findObject("AutoSeed").findGroup("PolygonSeederAlgorithm").addKeyword(PvlKeyword("MinEmission", "18.0"));
  autoseedDef.findObject("AutoSeed").findGroup("PolygonSeederAlgorithm").addKeyword(PvlKeyword("MaxEmission", "75.0"));
  autoseedDef.write(defFile);

  autoseed(autoseedUi, log);
  onet = ControlNet(outnet);
  ASSERT_EQ(onet.GetNumValidPoints(), 50);
  ASSERT_EQ(onet.GetNumValidMeasures(), 126);

  // Grid Pixels
  autoseedDef.findObject("AutoSeed").findGroup("PolygonSeederAlgorithm").deleteKeyword("MinEmission");
  autoseedDef.findObject("AutoSeed").findGroup("PolygonSeederAlgorithm").deleteKeyword("MaxEmission");
  autoseedDef.findObject("AutoSeed").findGroup("PolygonSeederAlgorithm").addKeyword(PvlKeyword("PixelsFromEdge", "20.0"));
  autoseedDef.write(defFile);

  autoseed(autoseedUi, log);
  onet = ControlNet(outnet);
  ASSERT_EQ(onet.GetNumValidPoints(), 57);
  ASSERT_EQ(onet.GetNumValidMeasures(), 144);

  // Grid Incidence
  autoseedDef.findObject("AutoSeed").findGroup("PolygonSeederAlgorithm").deleteKeyword("PixelsFromEdge");
  autoseedDef.findObject("AutoSeed").findGroup("PolygonSeederAlgorithm").addKeyword(PvlKeyword("MinIncidence", "35.0"));
  autoseedDef.findObject("AutoSeed").findGroup("PolygonSeederAlgorithm").addKeyword(PvlKeyword("MaxIncidence", "65.0"));
  autoseedDef.write(defFile);

  autoseed(autoseedUi, log);
  onet = ControlNet(outnet);
  ASSERT_EQ(onet.GetNumValidPoints(), 72);
  ASSERT_EQ(onet.GetNumValidMeasures(), 181);

  // Grid Resolution
  autoseedDef.findObject("AutoSeed").findGroup("PolygonSeederAlgorithm").deleteKeyword("MinIncidence");
  autoseedDef.findObject("AutoSeed").findGroup("PolygonSeederAlgorithm").deleteKeyword("MaxIncidence");
  autoseedDef.findObject("AutoSeed").findGroup("PolygonSeederAlgorithm").addKeyword(PvlKeyword("MinResolution", "255.0"));
  autoseedDef.findObject("AutoSeed").findGroup("PolygonSeederAlgorithm").addKeyword(PvlKeyword("MaxResolution", "259.0"));
  autoseedDef.write(defFile);

  autoseed(autoseedUi, log);
  onet = ControlNet(outnet);
  ASSERT_EQ(onet.GetNumValidPoints(), 60);
  ASSERT_EQ(onet.GetNumValidMeasures(), 147);
}
