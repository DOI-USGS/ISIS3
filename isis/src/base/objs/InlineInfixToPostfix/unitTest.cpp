/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <QDebug>

#include "InlineInfixToPostfix.h"
#include "Preference.h"
#include "IException.h"

/**
 * Test class that allows testing protected methods in InlineInfixToPostfix
 * 
 * @author 2016-02-24 Kristin Berry
 * 
 * @internal
 *   @history 2016-02-24 Kristin Berry - Original version (created for #2401).
 */
class TestInlineInfixToPostfix : public Isis::InlineInfixToPostfix {
  public:
    TestInlineInfixToPostfix() : InlineInfixToPostfix() {}

    bool isKnownSymbolWrap(QString rep) {
      return isKnownSymbol(rep);
    }

    Isis::InfixOperator *findOperatorWrap(QString elt) {
      return findOperator(elt); 
    }
};

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  TestInlineInfixToPostfix test; 

  std::cout << "Testing InlineInfixToPostfix::isKnownSymbol..." << std::endl; 

  std::cout << "+ should be True: " << test.isKnownSymbolWrap("+") << std::endl; 
  std::cout << "2 should be False: " << test.isKnownSymbolWrap("2") << std::endl; 
  std::cout << "a should be True (variable): " << test.isKnownSymbolWrap("a") << std::endl; 
  std::cout << "Empty string should be False: " << test.isKnownSymbolWrap("") << std::endl; 
  std::cout << "mod should be true: " << test.isKnownSymbolWrap("mod") << std::endl; 

  std::cout << "Testing InlineInfixToPostfix::findOperator..." << std::endl; 

  std::cout << "Valid input equation strings should have some output: " << std::endl; 
  qDebug() << test.findOperatorWrap("2 + 7")->inputString(); 
  qDebug() << test.findOperatorWrap("2 - 7")->outputString(); 
  qDebug() << test.findOperatorWrap("2 a 7")->inputString(); 

  std::cout << "Invalid input strings should throw an exception: " << std::endl; 
  
  try {
    qDebug() << test.findOperatorWrap("")->inputString(); 
  } 
  catch (Isis::IException &e) {
    e.print();
  }
}
