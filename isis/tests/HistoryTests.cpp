#include "Fixtures.h"
#include "History.h"

#include "gmock/gmock.h"

using namespace Isis;

TEST(HistoryTests, HistoryTestsDefaultConstructor) {
  History history;

  Pvl historyPvl  = history.ReturnHist();
  EXPECT_EQ(historyPvl.groups(), 0);
}

TEST_F(HistoryBlob, HistoryTestsFromBlob) {
  History readHistory(historyBlob);

  Pvl historyPvl = readHistory.ReturnHist();
  ASSERT_TRUE(historyPvl.hasObject("mroctx2isis"));
  EXPECT_TRUE(historyPvl.findObject("mroctx2isis").hasGroup("UserParameters"));
}

TEST_F(HistoryBlob, HistoryTestsAddEntry) {
  History history;

  history.AddEntry(historyPvl);

  Pvl newHistoryPvl = history.ReturnHist();
  ASSERT_TRUE(newHistoryPvl.hasObject("mroctx2isis"));
  EXPECT_TRUE(newHistoryPvl.findObject("mroctx2isis").hasGroup("UserParameters"));
}

TEST_F(HistoryBlob, HistoryTeststoBlob) {
  History history(historyBlob);

  Blob *blob = history.toBlob();

  std::stringstream os;
  char *blob_buffer = blob->getBuffer();
  Pvl newHistoryPvl;
  for (int i = 0; i < blob->Size(); i++) {
    os << blob_buffer[i];
  }
  os >> newHistoryPvl;

  ASSERT_TRUE(newHistoryPvl.hasObject("mroctx2isis"));
  EXPECT_TRUE(newHistoryPvl.findObject("mroctx2isis").hasGroup("UserParameters"));
}
