🏗 
## Why refactor?

In order to better integrate with the gtest unit test framework, all of our application logic needs to be callable programmatically.

This is a first step towards several places:
1. Improving granularity of application testing
1. Reducing testData size
1. Improving application test run time

## Creating a basic callable function

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

### If your application uses `Application::Log()`
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

## Creating a more complex callable function

The basic interface that we've created so far is simply a mirror of the application command line interface. In order to further improve the testability of the application, we should break the application down into 