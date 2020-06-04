#include <iostream>

#include <QHash>
#include <QSet>
#include <QString>

#include <boost/foreach.hpp>

#include "Pvl.h"
#include "Fixtures.h"
#include "gmock/gmock.h"
#include "UserInterface.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/cnetcombinept.xml").expanded();

QSet<QString> combineMerges(QHash<QString, QSet<QString>> &mergeLog, QSet<QString> newMerges) {
  QSet<QString> combinedMerges(newMerges);
  BOOST_FOREACH ( QString pointId, newMerges ) {
    // Recursively append everything previously merged into this point
    if (mergeLog.contains(pointId)) {
      combinedMerges.unite(combineMerges(mergeLog, mergeLog.take(pointId)));
    }
  }
  return combinedMerges;
}

TEST(FunctionalTestFunctionalTestsCnetCombinePt, combineMerges) {
  QHash<QString, QSet<QString>> testLog;
  testLog.insert("A", {"B"});
  testLog.insert("B", {"C", "D"});
  testLog.insert("D", {"E", "A"});
  testLog.insert("G", {"B", "E"});
  testLog.insert("H", {"I", "J", "K"});

  QSet<QString> mergedResult = combineMerges(testLog, {"A", "G"});
  EXPECT_EQ(mergedResult.size(), 6);
  EXPECT_TRUE(mergedResult.contains("A"));
  EXPECT_TRUE(mergedResult.contains("B"));
  EXPECT_TRUE(mergedResult.contains("C"));
  EXPECT_TRUE(mergedResult.contains("D"));
  EXPECT_TRUE(mergedResult.contains("E"));
  EXPECT_TRUE(mergedResult.contains("G"));

  EXPECT_EQ(testLog.size(), 1);
  ASSERT_TRUE(testLog.contains("H"));
  QSet<QString> hMerges = {"I", "J", "K"};
  EXPECT_EQ(testLog["H"], hMerges);
}
