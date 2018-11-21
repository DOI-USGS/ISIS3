#ifndef TestUtilities_h
#define TestUtilities_h

#include "gtest/gtest.h"

#include <string>

#include <QString>

#include "IException.h"

namespace Isis {

  /**
   * Custom IException assertion that checks the exception message contains
   * a substring and that the error type is correct.
   */
  ::testing::AssertionResult AssertIException(
        const char* e_expr,
        const char* contents_expr,
        const char* errorCode_expr,
        IException &e,
        QString contents,
        int errorCode) {
    if ( !e.toString().contains(contents) ) {
      return ::testing::AssertionFailure() << "IException " << e_expr
          << "\'s error message (" << e.toString().toStdString()
          << ") does not contain " << contents_expr << " ("
          << contents.toStdString() << ").";
    }

    if( e.errorType()!=errorCode ) {
      return ::testing::AssertionFailure() << "IException " << e_expr
          << "\'s error code (" << std::to_string(e.errorType())
          << ") does not equal " << errorCode_expr << " ("
          << std::to_string(errorCode) << ").";
    }

    return ::testing::AssertionSuccess();
  }

  /**
   * Custom QString assertion that properly outputs them as string if they
   * are different.
   */
  ::testing::AssertionResult AssertQStringsEqual(
        const char* string1_expr,
        const char* string2_expr,
        QString string1,
        QString string2) {
    if (string1 != string2) {
      return ::testing::AssertionFailure() << "QStrings " << string1_expr
          << " (" << string1.toStdString() << ") and " << string2_expr
          << " (" << string2.toStdString() << ") are not the same.";
    }

    return ::testing::AssertionSuccess();
  }
}

#endif
