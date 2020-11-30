#include <QTextStream>
#include <QStringList>
#include <QTemporaryFile>

#include "mapmos.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/mapmos.xml").expanded();

TEST_F(ThreeImageNetwork, FunctionalTestMapmosDefault) {
  QTemporaryDir prefix;
  QVector<QString> args = { "FROM=" + cube1->fileName(),
                            "MOSAIC=" + prefix.path() + "/mapmosOut.cub",
                            "create=true",
                            "track=true",
                            "priority=BAND",
                            "criteria=LESSER",
                            "minlat=true",
                            "maxlat=true",
                            "minlon=true",
                            "maxlon=true",
                            "highsaturation=true",
                            "lowsaturation=true",
                            "null=true" };
  UserInterface options(APP_XML, args);
  Pvl appLog;


  cube1->addCachingAlgorithm(new UniqueIOCachingAlgorithm(2));
  mapmos(options, &appLog);

}
