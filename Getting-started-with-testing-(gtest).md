## Creating a test suite for a class
1. Create a new file `ClassNameTests.cpp` in `isis/tests/`.
1. Add `#include <gtest/gtest.h>` to the file.
1. Write your test cases.

Note: If your tests require the use of ISIS' Null value, you will need to include SpecialPixel.h

## General Testing Tips

### Testing exceptions
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

### Testing floating point numbers
Comparison between floating point numbers is a common problem in both writing and testing software. Carefully choosing test data can help avoid comparisons that are likely to cause problems. Whenever possible, use test data such that output values can be exactly stored in a double-precision floating point number (c++ double). For example, if a function takes its input and divides it by 116, a good test input would be 232 because 232/116 = 2 which can be stored exactly in a double. On the other hand, 1 would be a poor test input because it cannot be precisely stored in a double. If a comparison between numbers that cannot be stored precisely is required, gtest provides [Special Assertions](https://github.com/abseil/googletest/blob/master/googletest/docs/advanced.md#floating-point-comparison) to make the comparisons more tolerant to different rounding.

### Test fixtures
Because each test case is a completely fresh environment, there is often set-up code that is shared between multiple test cases. You can use a test fixture to avoid copying and pasting this set-up code multiple times. See the [gtest documentation](https://github.com/abseil/googletest/blob/master/googletest/docs/primer.md#test-fixtures-using-the-same-data-configuration-for-multiple-tests) for how to create and use fixtures in gtest.

In order to conform to our test case naming conventions, all test fixtures need to be named `ClassName_FixtureName`. For example, a fixture that sets up a BundleSettings object in a not-default state for the BundleSettings unit test could be called `BundleSettings_NotDefault`.

### Test parameterization
If the same process needs to be run with several different inputs, it can be easily automated via [value-parameterized tests](https://github.com/abseil/googletest/blob/master/googletest/docs/advanced.md#value-parameterized-tests). In order to maintain test naming conventions, when you call the `INSTANTIATE_TEST_CASE_P` macro make sure the first parameter is. `ClassName`. For example

```
INSTANTIATE_TEST_CASE_P(
      BundleSettings,
      ConvergenceCriteriaTest,
      ::testing::Values(BundleSettings::Sigma0, BundleSettings::ParameterCorrections));
```

will ensure that the tests start with `BundleSettings`.

## Test names
We use gtest as our unit testing framework, but use ctest to actually run the tests. ctest will generate a test name from each test case defined in your unit test. The documentation for how this works can be found in [cmake's documentation](https://cmake.org/cmake/help/v3.13/module/GoogleTest.html).

To run the gtests for a specific class, use `ctest -R ClassName`

### Basic tests
`TEST(Foo, Bar)` will produce the test `Foo.Bar`.

### Test fixtures
`TEST_F(Foo, Bar)` will also produce the test `Foo.Bar`.

### Parameterized tests
```
TEST_P(Foo, Bar) {
...
}
INSTANTIATE_TEST_CASE_P(
      Baz,
      Foo,
      ::testing::Values(T x, T y, T z));
```
will produce 3 different tests that are named
1. `Baz/Foo.Bar/x`
1. `Baz/Foo.Bar/y`
1. `Baz/Foo.Bar/z`

## Helpful documentation
* [gtest primer](https://github.com/abseil/googletest/blob/master/googletest/docs/primer.md) : It is highly recommended that you read over the primer if you are not familiar with gtest.
* [Advanced gtest](https://github.com/abseil/googletest/blob/master/googletest/docs/advanced.md) : This document covers a wide range of advanced options for testing tricky logic and customizing test output.
* [gmock Introduction](https://github.com/abseil/googletest/blob/master/googlemock/docs/ForDummies.md) : A great introduction to how gmock can be used to remove dependencies from tests.
* [gmock Matcher List](https://github.com/abseil/googletest/blob/master/googlemock/docs/CheatSheet.md#matchers) : This whole document is helpful for gmock, but these matchers have some nice uses in gtest too.