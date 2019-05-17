#ifndef TestUtilities_h
#define TestUtilities_h

#include "gtest/gtest.h"

#include <string>

#include <QString>

#include "IException.h"

namespace Isis {

  ::testing::AssertionResult AssertIExceptionMessage(
          const char* e_expr,
          const char* contents_expr,
          IException &e,
          QString contents);

  ::testing::AssertionResult AssertIExceptionError(
          const char* e_expr,
          const char* errorType_expr,
          IException &e,
          IException::ErrorType errorType);

  ::testing::AssertionResult AssertQStringsEqual(
        const char* string1_expr,
        const char* string2_expr,
        QString string1,
        QString string2);
}

#endif
