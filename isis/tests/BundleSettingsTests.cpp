#include "BundleSettings.h"

#include <gtest/gtest.h>

class BoolTest : public ::testing::TestWithParam<bool> {
  // Intentionally empty
};

TEST_P(BoolTest, validateNetwork) {
  BundleSettings testSettings;
  testSettings.setValidateNetwork(GetParam());
  EXPECT_EQ(GetParam(), testSettings.validateNetwork());
}

INSTANTIATE_TEST_CASE_P(BundleSettings,
                        BoolTest,
                        ::testing::Bool());
