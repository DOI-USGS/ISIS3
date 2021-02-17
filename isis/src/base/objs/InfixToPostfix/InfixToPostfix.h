/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#ifndef INFIXTOPOSTFIX_H_
#define INFIXTOPOSTFIX_H_

#include <stack>
#include <iostream>

#include <QList>
#include <QString>

namespace Isis {
  class InfixOperator;
  class InfixFunction;

  /**
   * @brief Converter for math equations
   *
   * This class converts infix equations to postfix
   *
   * @ingroup Math
   *
   * @author 2007-08-21 Steven Lambright
   *
   * @internal
   *   @history 2007-08-22 Steven Lambright - Changed p_operators from
   *                           vector to QList (static std::vector
   *                           is unsafe)
   *   @history 2008-01-23 Steven Lambright - Added the operators (constants)
   *                           PI and e
   *   @history 2010-02-23 Steven Lambright - Operator/Function input strings and
   *                           output strings now separated
   *   @history 2010-04-08 Steven Lambright - Min, max functions expanded upon
   *   @history 2012-01-09 Jeff Anderson - Modified to conform
   *                           ISIS programming standards
   *   @history 2017-01-09 Jesse Mapel - Modified to allow for "_" in variables.
   *                           Fixes #4581.
   *   @history 2017-01-09 Jesse Mapel - Added logical and, or operators.
   *                           Fixes #4581.
   */
  class InfixToPostfix {
    public:
      InfixToPostfix();
      virtual ~InfixToPostfix();

      QString convert(const QString &infix);
      QString tokenizeEquation(const QString &equation);

    protected:

      virtual bool isKnownSymbol(QString representation);
      virtual InfixOperator *findOperator(QString representation);

      QList<InfixOperator *> p_operators;

    private:
      void initialize();
      void uninitialize();

      QString formatFunctionCalls(QString equation);
      QString cleanSpaces(QString equation);

      void closeParenthesis(QString &postfix, std::stack<InfixOperator> &theStack);
      void addOperator(QString &postfix, const InfixOperator &op, std::stack<InfixOperator> &theStack);
      bool isFunction(QString representation);
      void checkArgument(QString funcName, int argNum, QString argument);
  };

  /**
   * InfixOperator and InfixFunction are helper classes for InfixToPostfix
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class InfixOperator {
    public:
      InfixOperator(int prec, QString inString, bool isFunc = false) {
        m_precedence = prec;
        m_inputString = inString;
        m_outputString = inString;
        m_isFunction = isFunc;
      }

      InfixOperator(int prec, QString inString, QString outString,
                    bool isFunc = false) {
        m_precedence = prec;
        m_inputString = inString;
        m_outputString = outString;
        m_isFunction = isFunc;
      }

      const QString &inputString() const {
        return m_inputString;
      }

      const QString &outputString() const {
        return m_outputString;
      }

      int precedence() const {
        return m_precedence;
      }

      bool isFunction() const {
        return m_isFunction;
      }


    private:
      int m_precedence;
      QString m_inputString;
      QString m_outputString;
      bool m_isFunction;
  };


  /**
   * InfixOperator and InfixFunction are helper classes for InfixToPostfix
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class InfixFunction : public InfixOperator {
    public:
      InfixFunction(QString inString, int argCount) :
        InfixOperator(-1, inString, true) {
        m_numArguments = argCount;
      }

      InfixFunction(QString inString, QString outString, int argCount) :
        InfixOperator(-1, inString, outString, true) {
        m_numArguments = argCount;
      }

      int argumentCount() const {
        return m_numArguments;
      }

    private:
      int m_numArguments;
  };
};

#endif
