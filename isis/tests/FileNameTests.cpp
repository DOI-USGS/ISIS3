#include "FileName.h"

#include <QString>

#include <gtest/gtest.h>


TEST(FileNameTests, BaseName) {
  QString test = "test.log";
  Isis::FileName file(test);
  
  EXPECT_EQ("test", file.baseName());
}

int main(int argc, char **argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
