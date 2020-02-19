#include <iostream>
#include <QTemporaryFile>
#include <QTemporaryDir>

#include "autoseed.h"
#include "findimageoverlaps.h"
#include "footprintinit.h"

#include "Cube.h"
#include "CubeAttribute.h"
#include "ImageOverlapSet.h"
#include "PixelType.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "TestUtilities.h"
#include "FileName.h"

#include "Fixtures.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString FOOTPRINT_XML = FileName("$ISISROOT/bin/xml/footprintinit.xml").expanded();
static QString OVERLAP_XML = FileName("$ISISROOT/bin/xml/findimageoverlaps.xml").expanded();
static QString AUTOSEED_XML = FileName("$ISISROOT/bin/xml/autoseed.xml").expanded();

TEST_F(ThreeImageNetwork, FunctionalAutoseedDefault) {
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

  cubeList->removeLast();
  cubeList->write(cubeListTempPath.fileName());

  QTemporaryFile overlapList;
  overlapList.open();
  QVector<QString> overlapArgs = {"fromlist="+cubeListTempPath.fileName(),
                                  "overlaplist="+overlapList.fileName()};
  UserInterface overlapUi(OVERLAP_XML, overlapArgs);
  findimageoverlaps(overlapUi);

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
                                    "overlaplist="+overlapList.fileName(),
                                    "networkid=1",
                                    "pointid=??",
                                    "description=autoseed test network"};
  UserInterface autoseedUi(AUTOSEED_XML, autoseedArgs);

  autoseed(autoseedUi, log);
  ControlNet onet(outnet);
  ASSERT_EQ(onet.GetNumPoints(), 18);

  cubeList->append(cube3->fileName());
  cubeList->write(cubeListTempPath.fileName());
  findimageoverlaps(overlapUi);

  autoseed(autoseedUi, log);
  onet = ControlNet(outnet);
  ASSERT_EQ(onet.GetNumPoints(), 26);
}
