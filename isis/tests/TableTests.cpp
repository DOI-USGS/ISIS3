#include "Blob.h"
#include "TempFixtures.h"
#include "Table.h"
#include "TableField.h"
#include "TableRecord.h"

#include "gmock/gmock.h"

using namespace Isis;

TEST(TableTests, RecordConstructor) {
  TableField f1("Column1", TableField::Integer);
  TableField f2("Column2", TableField::Double);
  TableField f3("Column3", TableField::Text, 10);
  TableField f4("Column4", TableField::Double);
  TableRecord rec;
  rec += f1;
  rec += f2;
  rec += f3;
  rec += f4;
  Table t("UNITTEST", rec);

  EXPECT_EQ(t.RecordFields(), rec.Fields());
  EXPECT_EQ(t.RecordSize(), rec.RecordSize());
}


TEST(TableTests, Association) {
  Table t("UNITTEST");

  // Default initialization should be no association
  EXPECT_FALSE(t.IsSampleAssociated());
  EXPECT_FALSE(t.IsLineAssociated());
  EXPECT_FALSE(t.IsBandAssociated());

  t.SetAssociation(Table::Samples);
  EXPECT_TRUE(t.IsSampleAssociated());
  EXPECT_FALSE(t.IsLineAssociated());
  EXPECT_FALSE(t.IsBandAssociated());

  t.SetAssociation(Table::Lines);
  EXPECT_FALSE(t.IsSampleAssociated());
  EXPECT_TRUE(t.IsLineAssociated());
  EXPECT_FALSE(t.IsBandAssociated());

  t.SetAssociation(Table::Bands);
  EXPECT_FALSE(t.IsSampleAssociated());
  EXPECT_FALSE(t.IsLineAssociated());
  EXPECT_TRUE(t.IsBandAssociated());

  t.SetAssociation(Table::None);
  EXPECT_FALSE(t.IsSampleAssociated());
  EXPECT_FALSE(t.IsLineAssociated());
  EXPECT_FALSE(t.IsBandAssociated());
}


TEST(TableTests, UpdatingRecords) {
  TableField f1("Column1", TableField::Integer);
  TableField f2("Column2", TableField::Double);
  TableField f3("Column3", TableField::Text, 10);
  TableField f4("Column4", TableField::Double);
  TableRecord rec;
  rec += f1;
  rec += f2;
  rec += f3;
  rec += f4;
  Table t("UNITTEST", rec);

  rec[0] = 5;
  rec[1] = 3.14;
  rec[2] = "PI";
  rec[3] = 3.14159;
  t += rec;

  ASSERT_EQ(t.Records(), 1);
  EXPECT_EQ(TableRecord::toString(t[0]).toStdString(), TableRecord::toString(rec).toStdString());

  rec[0] = -1;
  rec[1] = 0.5;
  rec[2] = "HI";
  rec[3] = -0.55;
  t.Update(rec, 0);

  ASSERT_EQ(t.Records(), 1);
  EXPECT_EQ(TableRecord::toString(t[0]).toStdString(), TableRecord::toString(rec).toStdString());
}


TEST(TableTests, AddingRecords) {
  TableField f1("Column1", TableField::Integer);
  TableField f2("Column2", TableField::Double);
  TableField f3("Column3", TableField::Text, 10);
  TableField f4("Column4", TableField::Double);
  TableRecord rec;
  rec += f1;
  rec += f2;
  rec += f3;
  rec += f4;
  Table t("UNITTEST", rec);

  rec[0] = 5;
  rec[1] = 3.14;
  rec[2] = "PI";
  rec[3] = 3.14159;
  t += rec;

  ASSERT_EQ(t.Records(), 1);
  EXPECT_EQ(TableRecord::toString(t[0]).toStdString(), TableRecord::toString(rec).toStdString());

  rec[0] = -1;
  rec[1] = 0.5;
  rec[2] = "HI";
  rec[3] = -0.55;
  t += rec;

  ASSERT_EQ(t.Records(), 2);
  EXPECT_EQ(TableRecord::toString(t[1]).toStdString(), TableRecord::toString(rec).toStdString());
}


TEST(TableTests, ToFromBlob) {
  TableField f1("Column1", TableField::Integer);
  TableField f2("Column2", TableField::Double);
  TableField f3("Column3", TableField::Text, 10);
  TableField f4("Column4", TableField::Double);
  TableRecord rec;
  rec += f1;
  rec += f2;
  rec += f3;
  rec += f4;
  Table t("UNITTEST", rec);

  t.SetAssociation(Table::Lines);

  rec[0] = 5;
  rec[1] = 3.14;
  rec[2] = "PI";
  rec[3] = 3.14159;
  t += rec;

  rec[0] = -1;
  rec[1] = 0.5;
  rec[2] = "HI";
  rec[3] = -0.55;
  t += rec;

  QString comment = "test comment";
  t.Label().addComment(comment);

  Blob tableBlob = t.toBlob();

  Table t2(tableBlob);

  EXPECT_EQ(t.RecordFields(), t2.RecordFields());
  EXPECT_EQ(t.RecordSize(), t2.RecordSize());
  EXPECT_EQ(t.IsSampleAssociated(), t2.IsSampleAssociated());
  EXPECT_EQ(t.IsLineAssociated(), t2.IsLineAssociated());
  EXPECT_EQ(t.IsBandAssociated(), t2.IsBandAssociated());
  EXPECT_EQ(t.Label().comments(), t2.Label().comments());

  ASSERT_EQ(t.Records(), t2.Records());
  for (int i = 0; i < t.Records(); i++) {
    EXPECT_EQ(TableRecord::toString(t[i]).toStdString(), TableRecord::toString(t2[i]).toStdString());
  }
}


TEST_F(TempTestingFiles, TableTestsWriteRead) {
  TableField f1("Column1", TableField::Integer);
  TableField f2("Column2", TableField::Double);
  TableField f3("Column3", TableField::Text, 10);
  TableField f4("Column4", TableField::Double);
  TableRecord rec;
  rec += f1;
  rec += f2;
  rec += f3;
  rec += f4;
  Table t("UNITTEST", rec);

  t.SetAssociation(Table::Lines);

  rec[0] = 5;
  rec[1] = 3.14;
  rec[2] = "PI";
  rec[3] = 3.14159;
  t += rec;

  rec[0] = -1;
  rec[1] = 0.5;
  rec[2] = "HI";
  rec[3] = -0.55;
  t += rec;

  QString tableFile = tempDir.path() + "/testTable.pvl";
  t.Write(tableFile);
  Blob tableBlob("UNITTEST", "Table", tableFile);
  Table t2(tableBlob);

  EXPECT_EQ(t.RecordFields(), t2.RecordFields());
  EXPECT_EQ(t.RecordSize(), t2.RecordSize());
  EXPECT_EQ(t.IsSampleAssociated(), t2.IsSampleAssociated());
  EXPECT_EQ(t.IsLineAssociated(), t2.IsLineAssociated());
  EXPECT_EQ(t.IsBandAssociated(), t2.IsBandAssociated());

  ASSERT_EQ(t.Records(), t2.Records());
  for (int i = 0; i < t.Records(); i++) {
    EXPECT_EQ(TableRecord::toString(t[i]).toStdString(), TableRecord::toString(t2[i]).toStdString());
  }

  Table t3("UNITTEST", tableFile);

  EXPECT_EQ(t.RecordFields(), t3.RecordFields());
  EXPECT_EQ(t.RecordSize(), t3.RecordSize());
  EXPECT_EQ(t.IsSampleAssociated(), t3.IsSampleAssociated());
  EXPECT_EQ(t.IsLineAssociated(), t3.IsLineAssociated());
  EXPECT_EQ(t.IsBandAssociated(), t3.IsBandAssociated());

  ASSERT_EQ(t.Records(), t3.Records());
  for (int i = 0; i < t.Records(); i++) {
    EXPECT_EQ(TableRecord::toString(t[i]).toStdString(), TableRecord::toString(t3[i]).toStdString());
  }

  Table t4("NOT_UNITTEST");

  EXPECT_ANY_THROW(Table("NOT_UNITTEST", tableFile));
}


TEST(TableTests, Assignment) {
  TableField f1("Column1", TableField::Integer);
  TableField f2("Column2", TableField::Double);
  TableField f3("Column3", TableField::Text, 10);
  TableField f4("Column4", TableField::Double);
  TableRecord rec;
  rec += f1;
  rec += f2;
  rec += f3;
  rec += f4;
  Table t("UNITTEST", rec);

  rec[0] = 5;
  rec[1] = 3.14;
  rec[2] = "PI";
  rec[3] = 3.14159;
  t += rec;

  rec[0] = -1;
  rec[1] = 0.5;
  rec[2] = "HI";
  rec[3] = -0.55;
  t += rec;

  Table t2 = t;

  EXPECT_EQ(t.Name().toStdString(), t2.Name().toStdString());
  EXPECT_EQ(t.RecordFields(), t2.RecordFields());
  EXPECT_EQ(t.RecordSize(), t2.RecordSize());
  EXPECT_EQ(t.IsSampleAssociated(), t2.IsSampleAssociated());
  EXPECT_EQ(t.IsLineAssociated(), t2.IsLineAssociated());
  EXPECT_EQ(t.IsBandAssociated(), t2.IsBandAssociated());

  ASSERT_EQ(t.Records(), t2.Records());
  for (int i = 0; i < t.Records(); i++) {
    EXPECT_EQ(TableRecord::toString(t[i]).toStdString(), TableRecord::toString(t2[i]).toStdString());
  }
}


TEST(TableTests, Clear) {
  TableField f1("Column1", TableField::Integer);
  TableField f2("Column2", TableField::Double);
  TableField f3("Column3", TableField::Text, 10);
  TableField f4("Column4", TableField::Double);
  TableRecord rec;
  rec += f1;
  rec += f2;
  rec += f3;
  rec += f4;
  Table t("UNITTEST", rec);

  rec[0] = 5;
  rec[1] = 3.14;
  rec[2] = "PI";
  rec[3] = 3.14159;
  t += rec;

  rec[0] = -1;
  rec[1] = 0.5;
  rec[2] = "HI";
  rec[3] = -0.55;
  t += rec;

  t.Clear();

  EXPECT_EQ(t.Records(), 0);
}
