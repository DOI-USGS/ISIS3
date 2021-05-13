/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#ifndef CUBEINFIXTOPOSTFIX_H_
#define CUBEINFIXTOPOSTFIX_H_

#include "InfixToPostfix.h"
#include <stack>
#include <iostream>
#include <QVector>

namespace Isis {

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
   *   @history 2010-02-23 Steven Lambright Updated to use InfixOperator class
   *                           method instead of direct access to
   *                           member
   *   @history 2012-02-02 Jeff Anderson - Added the Initialize
   *                           method and camera variables (phase,
   *                           incidence, etc) for a cube
   *   @history 2012-02-09 Jeff Anderson - Modified to conform to
   *                           ISIS programming standards
   */
  class CubeInfixToPostfix : public InfixToPostfix {
    public:
      CubeInfixToPostfix();
      ~CubeInfixToPostfix() {};

    protected:
      bool isKnownSymbol(QString representation);
      InfixOperator *findOperator(QString representation);

    private:
      void initialize();
  };
};

#endif
