#include "TestUtilities.h"

namespace Isis {

  /**
   * Custom IException assertion that checks that the exception message contains
   * a substring.
   */
  ::testing::AssertionResult AssertIExceptionMessage(
          const char* e_expr,
          const char* contents_expr,
          IException &e,
          QString contents) {
    if (e.toString().contains(contents)) return ::testing::AssertionSuccess();

    return ::testing::AssertionFailure() << "IExeption "<< e_expr << "\'s error message (\"" 
       << e.toString().toStdString() << "\") does not contain " << contents_expr << " (\"" 
       << contents.toStdString() << "\").";
  }
  
  
  /**
   * Custom IException assertion that checks that the exception is the expected 
   * IException::ErrorType.
   */
  ::testing::AssertionResult AssertIExceptionError(
          const char* e_expr,
          const char* errorType_expr,
          IException &e,
          IException::ErrorType errorType) {
    if (e.errorType() == errorType) return ::testing::AssertionSuccess();

    return ::testing::AssertionFailure() << "IExeption "<< e_expr << "\'s error type (" 
        << std::to_string(e.errorType()) << ") does not match expected error type (" 
        << std::to_string(errorType) << ").";
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
