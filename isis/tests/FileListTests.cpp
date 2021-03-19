#include <gtest/gtest.h>
#include <iostream>
#include "FileList.h"
#include "IException.h"
#include "TestUtilities.h"

TEST(FileList, NonExistantFileConstructor)
{
  try
  {
    Isis::FileList fl1(Isis::FileName("FakeFile"));
  }
  catch(Isis::IException &e)
  {
    EXPECT_THAT(e.what(), testing::HasSubstr("Unable to open [FakeFile]"));
  }
  catch(...)
  {
    FAIL() << "Expected an IException\"Unable to open [FakeFile]\"";
  }
}

TEST(FileList, FileNameConstructor)
{
  std::istringstream input(
  "/usgs/pkgs/isis3/isis/src/base/objs/FileList/FileList.cpp\n"
  "/usgs/pkgs/isis3/isis/src/base/objs/FileList/FileList.h\n"
  "#Comment\n"
  "unitTest.cpp\n"
  ">This will not be comment ignored\n"
  "\n"
  "^is a blank line, this line will not be ignored as a comment\n"
  "  Makefile\n"
  "  //Testing comment with prepended spaces\n"
  "\n"
  "#Above and below are for testing multiple blank lines\n"
  "\n"
  "\n"
  "FileList.h\n");
  std::ostringstream output;
  std::string expectedOutput = "/usgs/pkgs/isis3/isis/src/base/objs/FileList/FileList.cpp\n"
     "/usgs/pkgs/isis3/isis/src/base/objs/FileList/FileList.h\n"
     "unitTest.cpp\n>This\n^is\nMakefile\nFileList.h\n";
  Isis::FileList fl1(input);
  fl1.write(output);
  EXPECT_STREQ(expectedOutput.c_str(), output.str().c_str());
}

TEST(FileList, FileNameNoNewLine)
{
  std::istringstream input(
  "/usgs/pkgs/isis3/isis/src/base/objs/FileList/FileList.cpp\n"
  "/usgs/pkgs/isis3/isis/src/base/objs/FileList/FileList.h");
  std::ostringstream output;
  std::string expectedOutput = "/usgs/pkgs/isis3/isis/src/base/objs/FileList/FileList.cpp\n"
     "/usgs/pkgs/isis3/isis/src/base/objs/FileList/FileList.h\n";
  Isis::FileList fl1(input);
  fl1.write(output);
  EXPECT_STREQ(expectedOutput.c_str(), output.str().c_str());
}
