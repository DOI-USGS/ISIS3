🏗 
# Why refactor?

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