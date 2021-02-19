/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// Calculator.h
#ifndef CALCULATOR_H_
#define CALCULATOR_H_

#include "Buffer.h"

// I'm not sure how to forward declare the iterators
#include <QVector>

template<class T> class QStack;
template<class T> class QVector;

namespace Isis {
  /**
   * @brief Calculator for arrays
   *
   * This class is a RPN calculator on arrays.  It uses classic
   * push/pop/operator methods.  That is, push array1, push
   * array2, add, pop arrayResult.
   *
   * @ingroup Math
   *
   * @author 2007-04-01 Sean Crosby
   *
   * @internal
   *  @history 2007-06-11 Jeff Anderson - Fixed bug in
   *                          Push(Buffer) method.  NAN was not computed
   *                          properly.
   *  @history 2007-08-21 Steven Lambright - Moved the infix to postfix
   *                          conversion into its own class.
   *  @history 2008-01-28 Steven Lambright - Added more support for the
   *                          power operator
   *  @history 2008-03-28 Steven Lambright - Condensed math methods to
   *                          just call PerformOperation(...). Converted 
   *                          valarray's to vectors (in order to use iterators).
   *  @history 2008-06-18 Christopher Austin - Added as well as fixed
   *                          documentation
   *  @history 2010-02-23 Steven Lambright - Added Minimum2, Maximum2 and all
   *                          min/max operations now ignore special pixels.
   *  @history 2010-04-08 Steven Lambright - Made min, max have proper
   *                          implementations and vectors are now QVectors.
   *  @history 2017-08-30 Summer Stapleton - Updated documentation. References #4807.
   *  @history 2017-08-30 Tyler Wilson and Ian Humphrey - provided std:: namespace for isnan
   *                          to fix ambiguity error when using c++11. References #4809.
   *  @history 2018-09-27 Kaitlyn Lee - Fixed the cout in PrintTop() so that -nan is printed
   *                          as nan. Updated code up to standards. References #5520.
   */
  class Calculator {
    public:
      Calculator();   // Constructor
      //! Virtual Constructor
      virtual ~Calculator();

      // Math methods
      void Negative();
      void Multiply();
      void Add();
      void Subtract();
      void Divide();
      void Modulus();

      void Exponent();
      void SquareRoot();
      void AbsoluteValue();
      void Log();
      void Log10();

      void LeftShift();
      void RightShift();
      void MinimumPixel();
      void MaximumPixel();
      void MinimumLine();
      void MaximumLine();
      void Minimum2(); //!< Not implemented in Calculator.cpp
      void Maximum2(); //!< Not implemented in Calculator.cpp
      void GreaterThan();
      void LessThan();
      void Equal();
      void LessThanOrEqual();
      void GreaterThanOrEqual();
      void NotEqual();
      void And();
      void Or();

      void Sine();
      void Cosine();
      void Tangent();
      void Secant();
      void Cosecant();
      void Cotangent();
      void Arcsine();
      void Arccosine();
      void Arctangent();
      void Arctangent2();
      void SineH();
      void CosineH();
      void TangentH();
      void ArcsineH();
      void ArccosineH();
      void ArctangentH();

      // Stack methods
      void Push(double scalar);
      void Push(Buffer &buff);
      void Push(QVector<double> &vect);
      QVector<double> Pop(bool keepSpecials = false);
      void PrintTop();
      bool Empty();
      virtual void Clear();

    protected:
      void PerformOperation(QVector<double> &results,
                            QVector<double>::iterator arg1Start,
                            QVector<double>::iterator arg1End,
                            double operation(double));
      void PerformOperation(QVector<double> &results,
                            QVector<double>::iterator arg1Start,
                            QVector<double>::iterator arg1End,
                            QVector<double>::iterator arg2Start,
                            QVector<double>::iterator arg2End,
                            double operation(double, double));

      //! Returns the current stack size
      int StackSize();

    private:
      //! The current stack of arguments
      QStack< QVector<double> > * p_valStack;
  };
};

#endif
