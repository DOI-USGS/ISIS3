#include "FileName.h"

#include <QString>

#include <gtest/gtest.h>


TEST(FileNameTests, OriginalPath) {
  QString test = "/testy/mc/test/face/test.cub";
  Isis::FileName file(test);
  
  EXPECT_EQ("/testy/mc/test/face", file.originalPath());
}

TEST(FileNameTests, Path) {
  QString test = "/testy/mc/test/face/test.cub";
  Isis::FileName file(test);
  
  EXPECT_EQ("/testy/mc/test/face", file.path());
}

TEST(FileNameTests, Attributes) {
  QString test = "/testy/mc/test/face/test.cub";
  Isis::FileName file(test);
  
  EXPECT_EQ("", file.attributes());
  
  QString testAtt = "/testy/mc/test/face/test.cub+Bsq";
  Isis::FileName fileAtt(testAtt);
  
  EXPECT_EQ("Bsq", fileAtt.attributes());
}

TEST(FileNameTests, BaseName) {
  QString test = "/testy/mc/test/face/test.cub";
  Isis::FileName file(test);
  
  EXPECT_EQ("test", file.baseName());
}

TEST(FileNameTests, Name) {
  QString test = "/testy/mc/test/face/test.cub";
  Isis::FileName file(test);
  
  EXPECT_EQ("test.cub", file.name());
}

TEST(FileNameTests, Extension) {
  QString test = "/testy/mc/test/face/test.cub";
  Isis::FileName file(test);
  
  EXPECT_EQ("cub", file.extension());
}

//TODO How is this going to work? Before we used ISISROOT. Will that still work?

// TEST(FileNameTests, Expanded) {
//   QString test = "/testy/mc/test/face/test.cub";
//   Isis::FileName file(test);
//   
//   EXPECT_EQ("cub", file.expanded());
// }

TEST(FileNameTests, Original) {
  QString test = "$ISISROOT/testy/mc/test/face/test.cub";
  Isis::FileName file(test);
  
  EXPECT_EQ("$ISISROOT/testy/mc/test/face/test.cub", file.original());
}

TEST(FileNameTests, AddExtension) {
  QString test = "/testy/mc/test/face/test.cub";
  Isis::FileName file(test);
  
  EXPECT_EQ("txt", file.addExtension(".txt").extension());
}

TEST(FileNameTests, RemoveExtension) {
  QString test = "/testy/mc/test/face/test.cub";
  Isis::FileName file(test);
  std::cout << file.removeExtension().extension().toStdString() << std::endl;
  EXPECT_EQ("", file.removeExtension().extension());
}

TEST(FileNameTests, SetExtension) {
  QString test = "/testy/mc/test/face/test.cub";
  Isis::FileName file(test);
  
  EXPECT_EQ("log", file.setExtension("log").extension());
}

//TODO There are a lot of different tests for versioning. Do we want to replicate them all??

TEST(FileNameTests, isNotVersioned) {
  QString test = "/testy/mc/test/face/test.cub";
  Isis::FileName file(test);
  
  EXPECT_EQ(false, file.isVersioned());
}

TEST(FileNameTests, isQuestionMarksNoExtensionVersioned) {
  QString test = "/testy/mc/test/face/test??????";
  Isis::FileName file(test);
  
  EXPECT_EQ(true, file.isVersioned());
}

TEST(FileNameTests, isQuestionMarksExtensionVersioned) {
  QString test = "/testy/mc/test/face/test??????.cub";
  Isis::FileName file(test);
  
  EXPECT_EQ(true, file.isVersioned());
}

TEST(FileNameTests, isDDMMMYYYVersioned) {
  QString test = "/testy/mc/test/face/test{ddMMMyyyy}..cub";
  Isis::FileName file(test);
  
  EXPECT_EQ(true, file.isVersioned());
}

//TODO How do we want to deal with test data?

// TEST(FileNameTests, HighestVersion) {
//   QString test = "/testy/mc/test/face/test{ddMMMyyyy}..cub";
//   Isis::FileName file(test);
//   
//   EXPECT_EQ(true, file.isVersioned());
// }

//TODO How do we test equal operators?
