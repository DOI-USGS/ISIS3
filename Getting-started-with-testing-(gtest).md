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
Comparison between floating point numbers is a common problem in both writing and testing software. Carefully choosing test data can help avoid comparisons that are likely to cause problems. Whenever possible, use test data such that output values can be exactly stored in a double-precision floating point number (c++ double). For example, if a function takes its input and divides it by 116, a good test input would be 232 because 232/116 = 2 which can be stored exactly in a double. On the other hand, 1 would be a poor test input because 1/116 cannot be precisely stored in a double. If a comparison between numbers that cannot be stored precisely is required, gtest provides [Special Assertions](https://github.com/abseil/googletest/blob/master/googletest/docs/advanced.md#floating-point-comparison) to make the comparisons more tolerant to different rounding.

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

To run all gtests, use `ctest -R "\." -E "(_app_|_unit_)"`

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

# Refactoring ISIS3 Applications

In order to better integrate with the gtest unit test framework, all of our application logic needs to be callable programmatically.

This is a first step towards several places:
1. Improving granularity of application testing
1. Reducing testData size
1. Improving application test run time

# Creating a basic callable function

For the rest of this document we will use `appname` as the name of the application that is being worked on.

1. In the `appname` folder create two new files, `appname.cpp` and `appname.h`. These files are where the application logic will live.
1. In `appname.h` and `appname.cpp` create a new function in the `Isis` namespace with the following signature `void appname(UserInterface &ui)`.
1. Copy the contents of `IsisMain` in `main.cpp` into the new function.
1. Copy any helper functions or global variables from `main.cpp` into `appname.cpp`. So as to not pollute the `Isis` namespace and avoid redefining symbols, forward declare any helper function in `appname.cpp` and do not define them in `appname.h`.
1. Put all of the required includes in `appname.cpp` and `appname.h`.
1. Remove the call to get the UserInterface; it usually looks like `UserInterface &ui = Application::GetUserInterface();`.
1. In `main.ccp`, put the following

```C++
#include "Isis.h"

#include "UserInterface.h"
#include "appname.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  appname(ui);
}
```

## If your application uses `Application::Log()`
Due to how the Application singleton works, calling `Application::Log()` outside of an actual ISIS application currently causes a segmentation fault. To avoid this, modify the new `appname` function to return a Pvl that contains all of the PvlGroups that need to be logged instead of calling `Application::Log()`. Then, change your `main.cpp` to

```C++
#include "Isis.h"

#include "Application.h"
#include "Pvl.h"
#include "UserInterface.h"
#include "appname.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl results = appname(ui);
  for (int resultIndex = 0; resultIndex < results.groups(); resultIndex++) {
    Application::Log(results.group(resultIndex));
  }
}
```

# Creating a more complex callable function

The basic interface that we've created so far is simply a mirror of the application command line interface. In order to further improve the testability of the application, we should break the application down into functional components.

## Separating parameter parsing
The first step is to separate the UserInterface parsing from the program logic.

All of the UserInterface parsing should be done in the `appname(UserInterface &ui)` function. Then, all of the actual program logic should be moved into another function also called `appname`. The signature for this function can be quite complex and intricate. Several tips for defining the `appname` function signature are in the next sections, but there is not perfect way to do this for every application.

Once the `appname` function is defined, the `appname(UserInterface &ui)` function simply needs to call it with the appropriate parameters once it has parsed the UserInterface.

## Separating I/O

Most ISIS3 applications were designed to read their inputs from files and then output their results to the command line and/or files. Unfortunately, gtest is very poorly setup to test against files and the command line. To work around this, it is necessary to remove as much file and command line output from the new `appname` functions as possible. Here are some examples of how outputs can be separated from the application logic:

1. Anything that would be logged to the terminal should be simply returned. This way, it can be programmatically validated in gtest. This in fact already needs to be done because of [issues](#if-your-application-uses-applicationlog) with `Application::Log()`.
1. Anything that writes to a text file should be moved into a new function. Then, instead of passing a filename and opening that file in the function, pass an `ostream` pointer. This way, in the `appname(UserInterface &ui)` function, the file can be opened and an `ofstream` object can be passed in, but for testing a `ostringstream` object can be passed.
1. No input filenames should be passed as arguments. All files required by the program should be opened and converted into in-memory objects and then passed to the function. This will help eliminate the need for test data files in many applications. **Make sure that for Cubes the appropriate CubeAttributeInput and CubeAttributeOutput values are set!**
1. Any complex objects needed by the function should be passed in as pointers. This allows for them to be mocked using [gmock](https://github.com/abseil/googletest/blob/master/googlemock/docs/ForDummies.md). This helps eliminate the need for test data files and better isolates the test. If the input object is broken somehow, a mock object will still operate for this test and instead of getting two failures, only the actually broken object's test will fail.

## Process, helper functions, and global variables
Many ISIS3 applications make sure of the Process class and its sub-classes. While these classes make file I/O easier, they also tightly couple it to the application logic! Some of them can take objects like cubes instead of filenames, but not all of them. They may need to be refactored to use objects instead of filenames. For now, where-ever possible use objects, but it is acceptable to use filenames if an object cannot be used.

Many of the Process sub-classes use process functions to operate on cubes. These helper functions will need to be pushed into the ISIS3 library. There is a chance that there will be symbol conflicts due to multiple applications using functions with the same name. In this case, the function can simply have its name modified. This is also a natural point at which application logic can be segmented and tested separately.

Because of how the Process sub-classes use process functions, many older ISIS3 applications use global variables to pass additional parameters to helper functions. This can cause serious issues because applications are no longer self-contained executables. To help with this, ProcessByBrick and ProcessByLine have been modified to use a [lambda function with captures](https://en.cppreference.com/w/cpp/language/lambda#Lambda_capture) as the process function. All of the previously global variables can now be defined in the `appname` function and then captured in the lambda function.

For example, here's an app called checkerboard which generates an artificial cube with a checkerboard pattern. It requires access to the variable `size` to work but `ProcessByLine.StartProcess` only supports functions `Buffer` objects as input. Therefore, the app uses a global variable in order for the checkerboard function to have `size` in it's scope:
```C++
// original foo.cpp
#include "Isis.h"

// .. some other incs 

int size;

void do_process(Buffer &in, Buffer &out) {
  // do stuff with in, out and size
}

void isismain() {
  UserInterface &ui = Application::GetUserInterface();
  size = ui.GetInteger("SIZE");
  
  ProcessByLine p;

  p.SetInputCube("FROM");
  p.SetOutputCube("TO");
 
  p.StartProcess(do_process);
}
```

can be refactored using a lambdas which captures the size variable: 

```C++
// callable foo.cpp
#include "Isis.h"

// .. some other incs 

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  // int is defined in the scope of main
  int size = ui.GetInteger("SIZE");
  
  // lambda equivalent of the checkerboard function, capture is set to get 
  // all local variables by reference 
  auto do_process = [&](Buffer &in, Buffer &out)->void {
    // do stuff with in, out and size 
  };  

  ProcessByLine p;

  p.SetInputCube("FROM");
  p.SetOutputCube("TO");
 
  p.StartProcess(do_process);
}
```

## Helpful documentation
* [gtest primer](https://github.com/abseil/googletest/blob/master/googletest/docs/primer.md) : It is highly recommended that you read over the primer if you are not familiar with gtest.
* [Advanced gtest](https://github.com/abseil/googletest/blob/master/googletest/docs/advanced.md) : This document covers a wide range of advanced options for testing tricky logic and customizing test output.
* [gmock Introduction](https://github.com/abseil/googletest/blob/master/googlemock/docs/ForDummies.md) : A great introduction to how gmock can be used to remove dependencies from tests.
* [gmock Matcher List](https://github.com/abseil/googletest/blob/master/googlemock/docs/CheatSheet.md#matchers) : This whole document is helpful for gmock, but these matchers have some nice uses in gtest too.