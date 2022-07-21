#include "LidarControlPoint.h"

#include "TestUtilities.h"
#include "iTime.h"

#include <gtest/gtest.h>

using namespace Isis;

TEST(LidarControlPoint, EditLock) {
  LidarControlPoint lcp;
  lcp.SetEditLock(true);

  ASSERT_TRUE(lcp.IsEditLocked());

  EXPECT_EQ(lcp.setRange(12.0), ControlPoint::Status::PointLocked);
  EXPECT_EQ(lcp.setSigmaRange(100.0), ControlPoint::Status::PointLocked);
  EXPECT_EQ(lcp.setTime(iTime(42.0)), ControlPoint::Status::PointLocked);
}


TEST(LidarControlPoint, SettersGetters) {
  LidarControlPoint lcp;

  EXPECT_EQ(lcp.setRange(12.0), ControlPoint::Status::Success);
  EXPECT_EQ(lcp.range(), 12.0);

  EXPECT_EQ(lcp.setSigmaRange(100.0), ControlPoint::Status::Success);
  EXPECT_EQ(lcp.sigmaRange(), 100.0);

  EXPECT_EQ(lcp.setTime(iTime(42.0)), ControlPoint::Status::Success);
  EXPECT_EQ(lcp.time().Et(), iTime(42.0).Et());
}


TEST(LidarControlPoint, Simultaneous) {
  LidarControlPoint lcp;

  QString newSerial = "LRO/1/286265995:36824/NACL";
  EXPECT_EQ(lcp.addSimultaneous(newSerial), ControlPoint::Status::Success);
  QList < QString > simultaneousList = lcp.snSimultaneous();
  ASSERT_EQ(simultaneousList.size(), 1);
  EXPECT_EQ(simultaneousList[0].toStdString(), newSerial.toStdString());
}