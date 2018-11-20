#include "FileName.h"

#include <QString>

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <gtest/gtest.h>

using namespace Isis;

class FileName_Fixture_Versioned : public ::testing::TestWithParam<const char*> {
  // Intentionally empty
};

class FileName_Fixture_NotVersioned : public ::testing::TestWithParam<const char*> {
  // Intentionally empty
};


TEST(FileName, DefaultConstructor) {
  FileName file;

  EXPECT_EQ("", file.baseName());
  EXPECT_EQ("", file.name());
  EXPECT_EQ("", file.extension());
}

// The other constructors are being tested in the other tests so there is no need to duplicate them

TEST(FileName, CopyConstructor) {
  FileName file;

  EXPECT_EQ("", file.baseName());
  EXPECT_EQ("", file.name());
  EXPECT_EQ("", file.extension());
}

TEST(FileName, OriginalPath) {
  QString test = "/testy/mc/test/face/test.cub";
  FileName file(test);

  EXPECT_EQ("/testy/mc/test/face", file.originalPath());
}

TEST(FileName, Path) {
  QString test = "/testy/mc/test/face/test.cub";
  FileName file(test);

  EXPECT_EQ("/testy/mc/test/face", file.path());
}

TEST(FileName, Attributes) {
  QString test = "/testy/mc/test/face/test.cub";
  FileName file(test);

  EXPECT_EQ("", file.attributes());

  QString testAtt = "/testy/mc/test/face/test.cub+Bsq";
  FileName fileAtt(testAtt);

  EXPECT_EQ("Bsq", fileAtt.attributes());
}

TEST(FileName, BaseName) {
  QString test = "/testy/mc/test/face/test.cub";
  FileName file(test);

  EXPECT_EQ("test", file.baseName());
}

TEST(FileName, Name) {
  QString test = "/testy/mc/test/face/test.cub";
  FileName file(test);

  EXPECT_EQ("test.cub", file.name());
}

TEST(FileName, Extension) {
  QString test = "/testy/mc/test/face/test.cub";
  FileName file(test);

  EXPECT_EQ("cub", file.extension());
}

TEST(FileName, Expanded) {
  QString relativeFileName("test.cub");
  FileName file("$ISISROOT/" + relativeFileName);
  QString isisRoot(getenv("ISISROOT"));
  EXPECT_EQ(isisRoot + "/" + relativeFileName, file.expanded());
}

TEST(FileName, Original) {
  QString test = "$ISISROOT/testy/mc/test/face/test.cub";
  FileName file(test);

  EXPECT_EQ("$ISISROOT/testy/mc/test/face/test.cub", file.original());
}

TEST(FileName, AddExtension) {
  QString test = "/testy/mc/test/face/test.cub";
  FileName file(test);

  EXPECT_EQ("txt", file.addExtension(".txt").extension());
}

TEST(FileName, RemoveExtension) {
  QString test = "/testy/mc/test/face/test.cub";
  FileName file(test);
  EXPECT_EQ("", file.removeExtension().extension());
}

TEST(FileName, SetExtension) {
  QString test = "/testy/mc/test/face/test.cub";
  FileName file(test);

  EXPECT_EQ("log", file.setExtension("log").extension());
}

TEST(FileName, isQuestionMarksNoExtensionVersioned) {
  QString test = "/testy/mc/test/face/test??????";
  FileName file(test);

  EXPECT_TRUE(file.isVersioned());
}

TEST(FileName, isQuestionMarksExtensionVersioned) {
  QString test = "/testy/mc/test/face/test??????.cub";
  FileName file(test);

  EXPECT_TRUE(file.isVersioned());
}

TEST(FileName, isDDMMMYYYVersioned) {
  QString test = "/testy/mc/test/face/test{ddMMMyyyy}..cub";
  FileName file(test);

  EXPECT_TRUE(file.isVersioned());
}

TEST(FileName, HighestVersion) {
  std::ofstream fileStream;
  fileStream.open("test000001.cub");
  fileStream << "test";
  fileStream.close();
  fileStream.open("test000002.cub");
  fileStream << "test";
  fileStream.close();

  FileName file("test??????.cub");
  EXPECT_EQ("./test000002.cub", file.highestVersion().expanded());
  remove("test000001.cub");
  remove("test000002.cub");

}

TEST(FileName, NewVersion) {
  std::ofstream fileStream;
  fileStream.open("test000001.cub");
  fileStream << "test";
  fileStream.close();
  fileStream.open("test000002.cub");
  fileStream << "test";
  fileStream.close();

  FileName file("test??????.cub");
  EXPECT_EQ("./test000003.cub", file.newVersion().expanded());
  remove("test000001.cub");
  remove("test000002.cub");
  remove("test000003.cub");
}

TEST(FileName, FileExists) {
  std::ofstream fileStream;
  fileStream.open("test000001.cub");
  fileStream << "test";
  fileStream.close();

  FileName realFile("test000001.cub");
  EXPECT_TRUE(realFile.fileExists());

  FileName fakeFile("test.cub");
  EXPECT_FALSE(fakeFile.fileExists());

  remove("test000001.cub");
}

TEST(FileName, CreateTempFile) {
  FileName test("test.cub");

  FileName temp = test.createTempFile(test);

  EXPECT_EQ("cub", temp.extension());
  EXPECT_TRUE(temp.fileExists());

  remove(temp.expanded().toLatin1());
}

TEST(FileName, ToString) {
  QString relativeFileName("test.cub");
  FileName file("$ISISROOT/" + relativeFileName);
  QString isisRoot(getenv("ISISROOT"));
  EXPECT_EQ(isisRoot + "/" + relativeFileName, file.toString());
}

TEST(FileName, AssignmentOperator) {
  FileName defaultFile("/testy/mc/test/face/test.cub");

  FileName file;
  file = defaultFile;

  EXPECT_EQ("test", file.baseName());
  EXPECT_EQ("test.cub", file.name());
  EXPECT_EQ("cub", file.extension());
}

TEST(FileName, EqualOperator) {
  FileName file1("/testy/mc/test/face/test.cub");
  FileName file2("/testy/mc/test/face/test.cub");

  EXPECT_TRUE(file1 == file2);
}

TEST(FileName, NotEqualOperator) {
  FileName file1("/testy/mc/test/face/test.cub");
  FileName file2("/testy/mc/test/face/Peaks.cub");

  EXPECT_TRUE(file1 != file2);
}

TEST_P(FileName_Fixture_Versioned, IsVersioned) {
  FileName file(GetParam());

  EXPECT_TRUE(file.isVersioned());
}

const char* versionedFiles[] = {"tttt??????", "tttt??????.tmp", "tttt_?.tmp", "??tttt",
                                "?tttt000008.tmp", "junk?", "tttt{ddMMMyyyy}.tmp",
                                "tt{MMM}tt{dd}yy{yy}.tmp", "tt{d}tt{MMM}.tmp", "tt{d}tt{MMMM}.tmp",
                                "tt{dd}.tmp", "tttt{dd}.tmp", "$TEMPORARY/{MMM}-{dd}-{yy}_v???.tmp"};

INSTANTIATE_TEST_CASE_P(FileName, FileName_Fixture_Versioned, ::testing::ValuesIn(versionedFiles));

TEST_P(FileName_Fixture_NotVersioned, IsVersioned) {
  FileName file(GetParam());

  EXPECT_FALSE(file.isVersioned());
}

const char* notVersionedFiles[] = {"tttt"};
//TODO These actually throw errors so they cannot be checked like this
//"tttt{}.tmp", "ttttt{}.tmp", "??tttt??", "tttt{aaaa}.tmp"};

INSTANTIATE_TEST_CASE_P(FileName, FileName_Fixture_NotVersioned, ::testing::ValuesIn(notVersionedFiles));
