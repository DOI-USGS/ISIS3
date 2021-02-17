#ifndef InlineInfixToPostfix_h
#define InlineInfixToPostfix_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// parent class, has InfixOperator type
#include "InfixToPostfix.h"

#include <QString>
#include <QStringList>

namespace Isis {
  /**
   * @brief A parser for converting equation strings to postfix.
   *
   * This class converts infix equations to postfix for parsing.
   *
   * @ingroup Math
   *  
   * @author 2012-07-15 Kris Becker
   * @internal
   *   @history 2012-07-15 Kris Becker - Original version.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-03-24 Jeffrey Covington and Jeannie Backer - Improved documentation.
   *   @history 2015-03-24 Jeannie Backer - Changed m_variables from a QVector
   *                           to QStringList
   *   @history 2016-02-21 Kristin Berry - Added unitTest.
   *   @history 2017-01-09 Jesse Mapel - Added logical and, or operators. Fixes #4581.
   */
  class InlineInfixToPostfix : public InfixToPostfix {
 
    public:
      InlineInfixToPostfix();
      virtual ~InlineInfixToPostfix();
 
    protected:
      virtual bool isKnownSymbol(QString representation);
      virtual InfixOperator *findOperator(QString element);
 
    private:
      void initialize();
      bool exists(const QString &str);
      bool isScalar(const QString &scalar);
      bool isVariable(const QString &str);
 
      QStringList m_variables; //!< The list of variables (represented as strings).
 
  };

} // Namespace Isis

#endif
