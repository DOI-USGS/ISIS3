#ifndef TempFixtures_h
#define TempFixtures_h

#include "gtest/gtest.h"

#include <QTemporaryDir>

namespace Isis {

  class TempTestingFiles : public ::testing::Test {
    protected:
      QTemporaryDir tempDir;

      void SetUp() override;
  };

}

#endif