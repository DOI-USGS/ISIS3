#include <QTextStream>
#include <QStringList>
#include <QFile>
#include <QDebug>

#include "lrowac2pds.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "SpecialPixel.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/lrowac2pds.xml").expanded();

TEST_F(ObservationPair, FunctionalTestLrowac2pdsDefault) {
  QVector<QString> args = {"fromlist=" + cubeListFile,
                           "to=" + tempDir.path() + "/LrowacTEMP.img"};
  UserInterface options(APP_XML, args);

  cubeL->label()->findObject("IsisCube").findGroup("Instrument")["InstrumentId"] = "WAC-VIS";

  lrowac2pds(options);
}
