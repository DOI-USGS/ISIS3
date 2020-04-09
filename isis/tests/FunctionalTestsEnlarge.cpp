#include "enlarge_app.h"

#include <QTemporaryFile>
#include <QTextStream>
#include <QStringList>

#include "Fixtures.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/enlarge.xml").expanded();

TEST_F(DefaultCube, FunctionalTestEnlargeDefaultParameters) {
  QVector<QString> args = {"to=" + tempDir.path()+"/output.cub", "sscale=2", "lscale=3"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  enlarge(testCube, options, &appLog);
  std::cout<<appLog<<std::endl;
}
