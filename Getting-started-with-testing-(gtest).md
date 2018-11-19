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
Testing for exception throws in gtest can be rather convoluted. Through testing and looking at what others have done, we have settled on using the following setup to test an exception throw:

```
try {
  // Code that should throw an IException
  FAIL() << "Expected an exception to be thrown";
}
catch(IException &e) {
  EXPECT_TRUE(e.toString().toLatin1().contains("Substring of the expected exception message"));
}
catch(...) {
  FAIL() << "Expected an IExcpetion with message message: \"The full expected exception message.\"";
}
```

Be sure to choose a substring of the exception message that will uniquely identify it, but only use parts of the string identified in the actual code. Anything like `**USER ERROR**` or `in IsisAml.cpp at 1613` that are added based on user preference settings should not be included in the substring. The IException and Preferences classes test that these are properly added onto the error messages.

Normally, if the error message does not contain the substring, gtest will only output the the expression evaluated to false. To make the failure message more helpful, we add `<< e.toString().toStdString()` which outputs the full error message if the test fails.

## Helpful Documentation
* [gtest primer](https://github.com/abseil/googletest/blob/master/googletest/docs/primer.md) : It is highly recommended that you read over the primer if you are not familiar with gtest.
* [Advanced gtest](https://github.com/abseil/googletest/blob/master/googletest/docs/advanced.md) : This document covers a wide range of advanced options for testing tricky logic and customizing test output.
* [gmock Introduction](https://github.com/abseil/googletest/blob/master/googlemock/docs/ForDummies.md) : A great introduction to how gmock can be used to remove dependencies from tests.
* [gmock Matcher List](https://github.com/abseil/googletest/blob/master/googlemock/docs/CheatSheet.md#matchers) : This whole document is helpful for gmock, but these matchers have some nice uses in gtest too.