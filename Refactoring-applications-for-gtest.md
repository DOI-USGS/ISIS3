🏗 
# Why refactor?

In order to better integrate with the gtest unit test framework, all of our application logic needs to be callable programmatically.

This is a first step towards several places:
1. Improving granularity of application testing
1. Reducing testData size
1. Improving application test run time

# Creating a basic callable function

For the rest of this document we will use `appname` as the name of the application that is being worked on. `AppName` will also be used as the upper camel case version of the application name.

1. In the `appname` folder create two new files, `AppNameFunc.cpp` and `AppNameFunc.h`. These files are where the application logic will live.
1. In `AppNameFunc.h` and `AppNameFunc.cpp` create a new function in the `Isis` namespace with the following signature `void appname(UserInterface &ui)`.
1. Copy the contents of `IsisMain` in `main.cpp` into the new function.
1. Copy any helper functions or global variables from `main.cpp` into `AppNameFunc.cpp`. So as to not pollute the `Isis` namespace and avoid redefining symbols, forward declare any helper function in `AppNameFunc.cpp` and do not define them in `AppNameFunc.h`.
1. Put all of the required includes in `AppNameFunc.cpp` and `AppNameFunc.h`.
1. Remove the call to get the UserInterface; it usually looks like `UserInterface &ui = Application::GetUserInterface();`.
1. In `main.ccp`, put the following

```
#include "Isis.h"

#include "UserInterface.h"
#include "AppNameFunc.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  appname(ui);
}
```

## If your application uses `Application::Log()`
Due to how the Application singleton works, calling `Application::Log()` outside of an actual ISIS application currently causes a segmentation fault. To avoid this, modify the new `appname` function to return a Pvl that contains all of the PvlGroups that need to be logged instead of calling `Application::Log()`. Then, change your `main.cpp` to

```
#include "Isis.h"

#include "Application.h"
#include "Pvl.h"
#include "UserInterface.h"
#include "AppNameFunc.h"

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

These new functions should not take filenames unless absolutely necessary. For example, anywhere an input cube is used, a Cube pointer should be passed and `appname(UserInterface &ui)` should be responsible for opening the input cube (Make sure the appropriate CubeAttributeInput and CubeAttributeOutput values are set). Any and all complex data should be passed into this function as a pointer so that they can be potentially mocked out in testing.

Once the `appname` functions are defined, the `appname(UserInterface &ui)` function simply needs to call the appropriate `appname` function once it has parsed the UserInterface.

### Separating I/O

Most ISIS3 applications were designed to read their inputs from files and then output their results to the command line and/or files. Unfortunately, gtest is very poorly setup to test against files and the command line. To work around this, it is necessary to remove as much file and command line output from the new `appname` functions as possible. Here are some examples of how outputs can be separated from the application logic:

1. Anything that would be logged to the terminal should be simply returned. This way, it can be programmatically validated in gtest. This in fact already needs to be done because of [issues with `Application::Log()`](#if-your-application-uses-applicationlog).
1. Anything that writes to a text file should be moved into a new function. Then, instead of passing a filename and opening that file in the function, pass an `ostream` pointer. This way, in the `appname(UserInterface &ui)` function, the file can be opened and an `ofstream` object can be passed in, but for testing a `ostringstream` object can be passed.
