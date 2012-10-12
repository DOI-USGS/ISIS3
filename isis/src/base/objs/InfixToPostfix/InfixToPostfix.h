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

#include "IString.h"
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
   *                           vector to QList (static std::vector
   *                           is unsafe)
   *   @history 2008-01-23 Steven Lambright - Added the operators (constants)
   *                           PI and e
   *   @history 2010-02-23 Steven Lambright - Operator/Function input strings and
   *                           output strings now separated
   *   @history 2010-04-08 Steven Lambright - Min, max functions expanded upon
   *   @history 2012-01-09 Jeff Anderson - Modified to conform
   *                           ISIS programming standards
   */
  class InfixToPostfix {
    public:
      InfixToPostfix();
      virtual ~InfixToPostfix();

      IString convert(const IString &infix);
      IString tokenizeEquation(const IString &equation);

    protected:

      virtual bool isKnownSymbol(IString representation);
      virtual InfixOperator *findOperator(IString representation);

      QList<InfixOperator *> p_operators;
    private:
      void initialize();
      void uninitialize();

      IString formatFunctionCalls(IString equation);
      IString cleanSpaces(IString equation);

      void closeParenthesis(IString &postfix, std::stack<InfixOperator> &theStack);
      void addOperator(IString &postfix, const InfixOperator &op, std::stack<InfixOperator> &theStack);
      bool isFunction(IString representation);
      void checkArgument(IString funcName, int argNum, IString argument);
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
      InfixOperator(int prec, IString inString, bool isFunc = false) {
        m_precedence = prec;
        m_inputString = inString;
        m_outputString = inString;
        m_isFunction = isFunc;
      }

      InfixOperator(int prec, IString inString, IString outString,
                    bool isFunc = false) {
        m_precedence = prec;
        m_inputString = inString;
        m_outputString = outString;
        m_isFunction = isFunc;
      }

      const IString &inputString() const {
        return m_inputString;
      }

      const IString &outputString() const {
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
      IString m_inputString;
      IString m_outputString;
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
      InfixFunction(IString inString, int argCount) :
        InfixOperator(-1, inString, true) {
        m_numArguments = argCount;
      }

      InfixFunction(IString inString, IString outString, int argCount) :
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
