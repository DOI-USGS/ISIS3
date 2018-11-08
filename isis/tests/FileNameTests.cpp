#include "FileName.h"

#include <QString>

#include <gtest/gtest.h>


TEST(FileNameTests, BaseName) {
  QString test = "test.log";
  Isis::FileName file(test);

  EXPECT_EQ("test", file.baseName());
}
