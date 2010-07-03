/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2010/04/08 15:14:13 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#ifndef INFIXTOPOSTFIX_H_
#define INFIXTOPOSTFIX_H_

#include "iString.h"
#include <stack>
#include <iostream>

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
   *                   vector to QList (static std::vector is unsafe)
   *   @history 2008-01-23 Steven Lambright - Added the operators (constants)
   *                   PI and e
   *   @history 2010-02-23 Steven Lambright - Operator/Function input strings and
   *                   output strings now separated
   *   @history 2010-04-08 Steven Lambright - Min, max functions expanded upon
   */
  class InfixToPostfix {
    public:
      InfixToPostfix();
      virtual ~InfixToPostfix();

      iString Convert(const iString &infix);
      iString TokenizeEquation(const iString &equation);

    protected:

      virtual bool IsKnownSymbol(iString representation);
      virtual InfixOperator *FindOperator(iString representation);

      QList<InfixOperator *> p_operators;
    private:
      void Initialize();
      void Uninitialize();

      iString FormatFunctionCalls(iString equation);
      iString CleanSpaces(iString equation);

      void CloseParenthesis(iString &postfix, std::stack<InfixOperator> &theStack);
      void AddOperator(iString &postfix, const InfixOperator &op, std::stack<InfixOperator> &theStack);
      bool IsFunction(iString representation);
      void CheckArgument(iString funcName, int argNum, iString argument);
  };

  /**
   * InfixOperator and InfixFunction are helper classes for InfixToPostfix
   */
  class InfixOperator {
    public:
      InfixOperator(int prec, iString inString, bool isFunc = false) {
        m_precedence = prec;
        m_inputString = inString;
        m_outputString = inString;
        m_isFunction = isFunc;
      }

      InfixOperator(int prec, iString inString, iString outString,
                    bool isFunc = false) {
        m_precedence = prec;
        m_inputString = inString;
        m_outputString = outString;
        m_isFunction = isFunc;
      }

      const iString &InputString() const {
        return m_inputString;
      }

      const iString &OutputString() const {
        return m_outputString;
      }

      int Precedence() const {
        return m_precedence;
      }

      bool IsFunction() const {
        return m_isFunction;
      }


    private:
      int m_precedence;
      iString m_inputString;
      iString m_outputString;
      bool m_isFunction;
  };

  class InfixFunction : public InfixOperator {
    public:
      InfixFunction(iString inString, int argCount) :
        InfixOperator(-1, inString, true) {
        m_numArguments = argCount;
      }

      InfixFunction(iString inString, iString outString, int argCount) :
        InfixOperator(-1, inString, outString, true) {
        m_numArguments = argCount;
      }

      int ArgumentCount() const {
        return m_numArguments;
      }

    private:
      int m_numArguments;
  };
};

#endif
