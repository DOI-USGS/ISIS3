#include <iostream>
#include <QTemporaryDir>

#include "cnetwinnow.h"

#include "PixelType.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "TestUtilities.h"
#include "FileName.h"
#include "SerialNumber.h"

#include "Fixtures.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/cnetwinnow.xml").expanded();

TEST_F(ThreeImageNetwork, CnetwinnowFunctionalDefault) {
  QTemporaryDir prefix;
  ASSERT_TRUE(prefix.isValid());

  QVector<QString> args = {"fromlist="+cubeListTempPath.fileName(),
                           "onet="+prefix.path()+"/winnowedNetwork.net",
                           "file_prefix=winnow"};
  UserInterface ui(APP_XML, args);

  int initialMeasureCount = network->GetNumValidMeasures();
  int initialPointCount = network->GetNumValidPoints();

  ASSERT_EQ(initialMeasureCount, 41);
  ASSERT_EQ(initialPointCount, 16);

  int count = 0;

  QList <ControlPoint*> pointList = network->GetPoints();

  QList <ControlMeasure*> measures;
  for (ControlPoint *point : pointList) {
    measures = point->getMeasures();
    for (ControlMeasure *measure: measures) {
      measure->SetResidual(count, count);
    }
    count++;
  }

  Progress progress;
  cnetwinnow(*network, progress, ui);

  int postWinnowMeasureCount = network->GetNumValidMeasures();
  int postWinnowPointCount = network->GetNumValidPoints();
  ASSERT_EQ(postWinnowMeasureCount, 31);
  ASSERT_EQ(postWinnowPointCount, 13);
}
