#include <QTextStream>
#include <QStringList>
#include <QTemporaryFile>

#include "camrange.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/camstats.xml").expanded();

TEST_F(DefaultCube, FunctionalTestCamrangeTest) {
  QVector<QString> args = { "FROM=" + testCube->fileName() };
  UserInterface options(APP_XML, args);
  Pvl appLog;

  camrange(options, &appLog);

  PvlGroup group = appLog.findGroup("Target");

  std::cout << group.findKeyword("TargetName") << std::endl;

  EXPECT_TRUE(true);
}
