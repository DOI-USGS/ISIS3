#include "FileName.h"

#include <QString>

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
  
//   EXPECT_EQ("", file.originalPath());
//   EXPECT_EQ("", file.path());
//   EXPECT_EQ("", file.attributes());
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

//TODO Waiting for GMock

// TEST(FileName, Expanded) {
// }

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
  std::cout << file.removeExtension().extension().toStdString() << std::endl;
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
  
  EXPECT_EQ(true, file.isVersioned());
}

TEST(FileName, isQuestionMarksExtensionVersioned) {
  QString test = "/testy/mc/test/face/test??????.cub";
  FileName file(test);
  
  EXPECT_EQ(true, file.isVersioned());
}

TEST(FileName, isDDMMMYYYVersioned) {
  QString test = "/testy/mc/test/face/test{ddMMMyyyy}..cub";
  FileName file(test);
  
  EXPECT_EQ(true, file.isVersioned());
}

//TODO Waiting for GMock

// TEST(FileName, HighestVersion) {
// }

// TEST(FileName, NewVersion) {
// }

// TEST(FileName, Version) {
// }

TEST(FileName, ToString) {
  QString test = "/testy/mc/test/face/test.cub";
  FileName file(test);
  
  EXPECT_EQ("/testy/mc/test/face/test.cub", file.toString());
}

// TODO Waiting for GMock
// TEST(FileName, EqualOperator) {
// }

// TEST(FileName, NotEqualOperator) {
// }

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
