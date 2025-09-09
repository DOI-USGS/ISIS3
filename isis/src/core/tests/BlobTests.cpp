#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QString>
#include <iostream>

#include "Pvl.h"
#include "Blob.h"
#include "SpecialPixel.h"

#include "gmock/gmock.h"

using namespace Isis;


class DefaultBlob : public ::testing::Test {

  protected:
    QTemporaryDir tempDir;
    Blob *testBlob;
    QString testBlobPath;

    void SetUp() override {
      ASSERT_TRUE(tempDir.isValid());
      testBlobPath = tempDir.path() + "/junk_blob.pvl";

      testBlob = new Blob("UnitTest", "Blob");
      char buf[] = {"ABCD"};
      testBlob->setData(buf, 4);
      testBlob->Write(testBlobPath);
    }

    void TearDown() override {
      delete testBlob;
    }
  
};

TEST_F(DefaultBlob, TestBlobDefault) {
  Blob b("UnitTest", "Blob");
  char buf[] = {"ABCD"};
  b.setData(buf, 4);
  b.Write("junk");

  EXPECT_EQ("UnitTest", b.Name());
  EXPECT_EQ(4, b.Size());
  EXPECT_EQ("Blob", b.Type());
}

TEST_F(DefaultBlob, TestBlobRead) {
  Blob b("UnitTest", "Blob", testBlobPath);
  
  EXPECT_EQ("UnitTest", b.Name());
  EXPECT_EQ(4, b.Size());
  EXPECT_EQ("Blob", b.Type());

  std::string buffStr(b.getBuffer());
  EXPECT_EQ("ABCD", buffStr);
}

TEST_F(DefaultBlob, TestBlobWriteExisting) {
  Isis::Pvl pvl(testBlobPath);
  std::fstream strm;
  strm.open(testBlobPath.toStdString(), std::ios::binary | std::ios::out);
  char buf[] = {"ABCD"};
  testBlob->setData(buf, 3);
  ASSERT_EQ("UnitTest", testBlob->Name());
  ASSERT_EQ(3, testBlob->Size());
  ASSERT_EQ("Blob", testBlob->Type());
  testBlob->Write(pvl, strm);
  strm.seekp(0, std::ios::beg);
  strm << pvl;
  strm.close();

  Blob b("UnitTest", "Blob", testBlobPath);

  EXPECT_EQ("UnitTest", b.Name());
  EXPECT_EQ(3, b.Size());
  EXPECT_EQ("Blob", b.Type());
}

TEST_F(DefaultBlob, TestBlobWriteExistingEOF) {
  Isis::Pvl pvl(testBlobPath);
  std::fstream strm;
  strm.open(testBlobPath.toStdString(), std::ios::binary | std::ios::out);
  char buf[] = {"ABCD"};
  testBlob->setData(buf, 3);
  ASSERT_EQ("UnitTest", testBlob->Name());
  ASSERT_EQ(3, testBlob->Size());
  ASSERT_EQ("Blob", testBlob->Type());
  testBlob->Write(pvl, strm);
  strm.seekp(0, std::ios::beg);
  strm << pvl;
  strm.close();

  pvl = Pvl(testBlobPath);
  strm.open(testBlobPath.toStdString(), std::ios::binary | std::ios::out);
  testBlob->setData(buf, 4);
  ASSERT_EQ("UnitTest", testBlob->Name());
  ASSERT_EQ(4, testBlob->Size());
  ASSERT_EQ("Blob", testBlob->Type());
  testBlob->Write(pvl, strm);
  strm.seekp(0, std::ios::beg);
  strm << pvl;
  strm.close();

  Blob b("UnitTest", "Blob", testBlobPath);

  EXPECT_EQ("UnitTest", b.Name());
  EXPECT_EQ(4, b.Size());
  EXPECT_EQ("Blob", b.Type());
}

TEST_F(DefaultBlob, TestBlobWriteReadGdal) {
  GDALDataset *dataset = NULL;
  QString path = tempDir.path() + "/tiny.tiff";
  GDALAllRegister();
  GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("GTiff");
  if (driver) {
    char **papszOptions = NULL;
    dataset = driver->Create(path.toStdString().c_str(), 1, 1, 1, GDT_Byte, papszOptions);
    if (dataset) {
      double noDataValue = (double) NULL1;
      GDALRasterBand *band = dataset->GetRasterBand(1);
      band->SetScale(1);
      band->SetOffset(0);
      band->SetNoDataValue(noDataValue);
      dataset->CreateMaskBand(GMF_ALPHA);
      dataset->GetRasterBand(1)->GetMaskBand()->Fill(255);
      dataset->Close();
    }
  }
  dataset = GDALDataset::FromHandle(GDALOpen(path.toStdString().c_str(), GA_Update));

  Blob writeBlob("UnitTest", "Blob");
  char buf[] = {"ABCD"};
  writeBlob.setData(buf, 4);

  std::string jsonblobstr = "{}";
  writeBlob.WriteGdal(jsonblobstr);

  char ** outputMetadata = new char*[1];
  outputMetadata[0] = jsonblobstr.data();
  dataset->SetMetadata(outputMetadata, "json:ISIS3");
  delete []outputMetadata;

  GDALClose(dataset);
  dataset = GDALDataset::FromHandle(GDALOpen(path.toStdString().c_str(), GA_Update));

  Blob readBlob("UnitTest", "Blob");
  readBlob.ReadGdal(dataset);

  EXPECT_EQ(writeBlob.Name(), readBlob.Name());
  EXPECT_EQ(writeBlob.Size(), readBlob.Size());
  EXPECT_EQ(writeBlob.Type(), readBlob.Type());

  std::string writeBuff(writeBlob.getBuffer());
  std::string readBuff(readBlob.getBuffer());

  EXPECT_EQ(writeBuff, readBuff);

  GDALClose(dataset);
}