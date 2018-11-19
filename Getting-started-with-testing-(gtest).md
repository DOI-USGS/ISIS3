:building_construction:

## Creating a test suite for a class
1. Create a new file `ClassNameTests.cpp` in `isis/tests/`.
1. Add `#include <gtest/gtest.h>` to the file.
1. Write your test cases.
1. Each test case should have the form `TEST(ClassName, MethodName)`. This produces a ctest test called `ClassName.MethodName`. For example, `TEST(BundleSettings, DefaultConstructor)` produces a ctest test called `BundleSettings.DefaultConstructor`.

## Test Fixtures
Because each test case is a completely fresh environment, there is often set-up code that is shared between multiple test cases. You can use a test fixture to avoid copying and pasting this set-up code multiple times. See the [gtest documentation](https://github.com/abseil/googletest/blob/master/googletest/docs/primer.md#test-fixtures-using-the-same-data-configuration-for-multiple-tests) for how to create and use fixtures in gtest.

In order to conform to our test case naming conventions, all test fixtures need to be named `ClassName_FixtureName`. For example, a fixture that sets up a BundleSettings object in a not-default state for the BundleSettings unit test could be called `BundleSettings_NotDefault`.

## Testing exceptions

1. For testing exceptions, use something like the following: 

```
   EXPECT_TRUE(e.toString().contains("PVL Keyword [CenterRingRadius] does not exist in "
                                      "[Group = Mapping]")) << e.toString().toStdString();
```

The `<< e.toString().toStdString()` provides a more helpful error message if the test fails.

## Helpful Documentation
* [gtest primer](https://github.com/abseil/googletest/blob/master/googletest/docs/primer.md) : It is highly recommended that you read over the primer if you are not familiar with gtest.
* [Advanced gtest](https://github.com/abseil/googletest/blob/master/googletest/docs/advanced.md) : This document covers a wide range of advanced options for testing tricky logic and customizing test output.
* [gmock Introduction](https://github.com/abseil/googletest/blob/master/googlemock/docs/ForDummies.md) : A great introduction to how gmock can be used to remove dependencies from tests.
* [gmock Matcher List](https://github.com/abseil/googletest/blob/master/googlemock/docs/CheatSheet.md#matchers) : This whole document is helpful for gmock, but these matchers have some nice uses in gtest too.