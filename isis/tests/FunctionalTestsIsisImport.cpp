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
//#include "makecube.h"
//#include "isis2std.h"

#include "gmock/gmock.h"

using namespace Isis;
using json = nlohmann::json;

static QString APP_XML = FileName("$ISISROOT/bin/xml/isisimport.xml").expanded();
static QString STD_APP_XML = FileName("$ISISROOT/bin/xml/isis2std.xml").expanded();
static QString MAKECUBE_APP_XML = FileName("$ISISROOT/bin/xml/makecube.xml").expanded();

TEST(IsisImport, FunctionalTestIsisImportLabelXmlInput) {
  QString labelFileName = "data/isisimport/temp.xml";
  QString imageFileName = "data/isisimport/temp.img";
  std::ofstream ofxml;
  ofxml.open(labelFileName.toStdString());
  ofxml << R"(<Dimensions> <Lines>2</Lines> <Samples>3</Samples> <Bands>1</Bands> </Dimensions>)";
  ofxml.close();

  QString templateFile = "data/isisimport/test_result.tpl";
  QString renderedCube = "data/isisimport/test_result.cub";
  std::ofstream of;
  of.open(templateFile.toStdString());
  of << R"(Object = IsisCube
  Object = Core
    StartByte   = 65537
    Format      = Tile
    TileSamples = 1
    TileLines   = 1

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

/*  QVector<QString> makeCubeArgs = {"lines=1", "samples=2", "bands=3", "to=input.cub"}; // update with args
  UserInterface makeCubeOptions(MAKECUBE_APP_XML, makeCubeArgs);
  makecube(makeCubeOptions);

  QVector<QString> stdArgs = {"from=input.cub", "to=" + imageFileName};
  UserInterface stdOption(STD_APP_XML, stdArgs);
  isis2std(stdOption); */
}

