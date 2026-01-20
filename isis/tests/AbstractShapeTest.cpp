#include "gtest/gtest.h"

#include "Preference.h"

using namespace Isis;

class AbstractShapeFixture : public ::testing::Test {
  protected:
    void SetUp() override {
      Preference::Preferences(true);
    }
};

TEST_F(AbstractShapeFixture, SmokeTest) {
  SUCCEED();
}
