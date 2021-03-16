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

TEST_F(HistoryBlob, HistoryTestsAddSecondEntry) {
  History history(historyBlob);

  std::istringstream hss(R"(
      Object = ctxcal
        IsisVersion       = "4.1.0  | 2020-07-01"
        ProgramVersion    = 2016-06-10
        ProgramPath       = /Users/acpaquette/repos/ISIS3/build/bin
        ExecutionDateTime = 2020-07-01T16:48:40
        HostName          = Unknown
        UserName          = acpaquette
        Description       = "Import an MRO CTX image as an Isis cube"

        Group = UserParameters
          FROM    = /Users/acpaquette/Desktop/J03_045994_1986_XN_18N282W_isis.cub
          TO      = /Users/acpaquette/Desktop/J03_045994_1986_XN_18N282W_isis.cal.cub
        End_Group
      End_Object)");

  PvlObject secondHistoryPvl;
  hss >> secondHistoryPvl;

  history.AddEntry(secondHistoryPvl);

  Blob blob = history.toBlob();

  Pvl newHistoryPvl = history.ReturnHist();

  EXPECT_TRUE(newHistoryPvl.hasObject("mroctx2isis"));

  ASSERT_TRUE(newHistoryPvl.hasObject("ctxcal"));
  EXPECT_TRUE(newHistoryPvl.findObject("ctxcal").hasGroup("UserParameters"));

  History reingestedHistory(blob);

  Pvl reingestedHistoryPvl = reingestedHistory.ReturnHist();

  EXPECT_TRUE(reingestedHistoryPvl.hasObject("mroctx2isis"));

  ASSERT_TRUE(reingestedHistoryPvl.hasObject("ctxcal"));
  EXPECT_TRUE(reingestedHistoryPvl.findObject("ctxcal").hasGroup("UserParameters"));
}

TEST_F(HistoryBlob, HistoryTeststoBlob) {
  History history(historyBlob);

  Blob blob = history.toBlob();

  std::stringstream os;
  char *blob_buffer = blob.getBuffer();
  Pvl newHistoryPvl;
  for (int i = 0; i < blob.Size(); i++) {
    os << blob_buffer[i];
  }
  os >> newHistoryPvl;

  ASSERT_TRUE(newHistoryPvl.hasObject("mroctx2isis"));
  EXPECT_TRUE(newHistoryPvl.findObject("mroctx2isis").hasGroup("UserParameters"));
}
