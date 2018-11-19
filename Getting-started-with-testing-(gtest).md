:building_construction:

## Creating a test suite for a class
1. Create a new file `ClassNameTests.cpp` in `isis/tests/`.
  1. Add `#include <gtest/gtest.h>` to the file
1. Each test case (`TEST()`) should have the form `ClassName, MethodName` or `ClassName_Fixture, MethodName`

1. For testing exceptions, use something like the following: 

```
   EXPECT_TRUE(e.toString().contains("PVL Keyword [CenterRingRadius] does not exist in "
                                      "[Group = Mapping]")) << e.toString().toStdString();
```

The `<< e.toString().toStdString()` provides a more helpful error message if the test fails. 