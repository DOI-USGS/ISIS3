#ifndef InlineCalculator_h
#define InlineCalculator_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// parent class
#include "Calculator.h"

#include <QList>
#include <QMap>
#include <QString>
#include <QVector>

class QVariant;

namespace Isis {

  class CalculatorVariablePool;
  class FxBinder;
  class ParamaterFx;
  class VoidFx;
 
  /**
   * Macro for calling member functions. Read all about it at
   *  http://www.parashift.com/c++-faq/pointers-to-members.html.
   */
  #define CALL_MEMBER_FN(object, ptrToMember)   ((object).*(ptrToMember))
 
  /**
   * @brief Provides a calculator for inline equations.
   *
   * A calculator with the ability to parse infix equations with embedded
   * variables and scalars, known as an inline equation.
   *  
   *  
   * @author 2012-07-15 Kris Becker
   * @internal
   *   @history 2012-07-15 Kris Becker - Original version.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-03-24 Jeffrey Covington and Jeannie Backer - Improved documentation.
   *   @history 2016-02-21 Kristin Berry - Added unit test and minor coding standard updates.
   *                                       Fixes #2401.
   *   @history 2017-01-09 Jesse Mapel - Added logical and, or operators. Fixes #4581.
   */
  class InlineCalculator : public Calculator {
 
    public:
 
      InlineCalculator();
      InlineCalculator(const QString &equation);
      virtual ~InlineCalculator();
 
      int size() const;
 
      QString equation() const;
      bool compile(const QString &equation);
 
      QVector<double> evaluate(CalculatorVariablePool *variablePool);
      QVector<double> evaluate();
 
    protected:
      //! Defintion for a FxTypePtr, a pointer to a function binder (FxBinder)
      typedef FxBinder *FxTypePtr;
 
      // Implementations of local/new functions
      void scalar(const QVariant &scalar);
      void variable(const QVariant &variable);
 
      void floatModulus();
      void radians();
      void degrees();
      void logicalOr();
      void logicalAnd();
      void pi();
      void eConstant();
 
      virtual QString toPostfix(const QString &equation) const;
      bool isScalar(const QString &scalar);
      bool isVariable(const QString &str);
 
      // Derived classes can be added with these methods for customization.
      // See FxBinder().
      bool fxExists(const QString &fxname) const;
      FxTypePtr addFunction(FxTypePtr function);
 
      virtual bool orphanTokenHandler(const QString &token);
 
    private:
      //! Definition for a FxEqList, a vector of function type pointers
      typedef QVector<FxTypePtr>       FxEqList;
      //! Definition for a FxPoolType, a map between a string and function type pointer
      typedef QMap<QString, FxTypePtr> FxPoolType;
 
      void pushVariables(CalculatorVariablePool *variablePool);
      CalculatorVariablePool *variables();
      void popVariables();
 
      FxTypePtr find(const QString &fxname);
      void initialize();
      void destruct();

      FxEqList    m_functions; //!< The list of pointers to function equations for the calculator.
      FxPoolType  m_fxPool;    //!< The map between function names and equation lists.
      QString     m_equation;  //!< The equation to be evaluated.
      QList<CalculatorVariablePool *> m_variablePoolList; //!< The list of variable pool pointers.
 
  };
 

  /**
   * This is a simple class to model a Calculator Variable Pool. 
   *  
   * @author 2012-07-15 Kris Becker
   * @internal
   *   @history 2012-07-15 Kris Becker - Original version.
   */
  class CalculatorVariablePool {
    public:
    CalculatorVariablePool();
    ~CalculatorVariablePool();
   
    virtual bool exists(const QString &variable) const;
    virtual QVector<double> value(const QString &variable,
                                  const int &index = 0) const;
    virtual void add(const QString &key, QVector<double> &values);
  };
 
 
  /**
   * This is the parent class to the various function classes. 
   *  
   * @author 2012-07-15 Kris Becker
   * @internal
   *   @history 2012-07-15 Kris Becker - Original version.
   */  
  class FxBinder {
    public:
      FxBinder(const QString &name);
      virtual ~FxBinder();
 
      QString name() const;
      void execute();
      void operator()();

      /**
       * This method defines how to execute this function. This class does not
       * define an implementation for this pure virtual method.
       */
      virtual void dispatch() = 0;
      virtual QVariant args();
 
    private:
      QString m_name; //!< Name of function
  };
 
 
  /**
   * This class is used to bind function names with corresponding
   * InlineCalculator functions that do not take parameters. 
   *  
   * @author 2012-07-15 Kris Becker
   * @internal
   *   @history 2012-07-15 Kris Becker - Original version.
   */  
  class InlineVoidFx : public FxBinder {
    public:
      //! Defines an InlineCalculator function that takes no arguments.
      typedef void (InlineCalculator::*calcOp)();
       
      InlineVoidFx(const QString &name, calcOp function,
                   InlineCalculator *calculator);
      virtual ~InlineVoidFx();
      void dispatch();
 
    private:
      calcOp  m_func;           //!< The InlineCalculator operator that takes no parameters.
      InlineCalculator *m_calc; //!< The InlineCalculator used to evaluate this function.
  };
 
 
  /**
   * This class is used to bind function names with corresponding Calculator
   * functions that take a parameter. 
   *  
   * @author 2012-07-15 Kris Becker
   * @internal
   *   @history 2012-07-15 Kris Becker - Original version.
   */  
  class ParameterFx : public FxBinder {
    public:
      //! Defines an InlineCalculator function that takes arguments.
      typedef void (InlineCalculator::*calcOp)(const QVariant &arg);
       
      ParameterFx(const QString &name, calcOp function,
                  InlineCalculator *calculator);
      virtual ~ParameterFx();
      void dispatch();
 
    private:
      calcOp  m_func;           //!< The InlineCalculator operator that takes parameters.
      InlineCalculator *m_calc; //!< The InlineCalculator used to evaluate this function.
  };


  /**
   * This class is used to bind function names with corresponding Calculator
   * functions that do not take parameters. 
   *  
   * @author 2012-07-15 Kris Becker
   * @internal
   *   @history 2012-07-15 Kris Becker - Original version.
   */  
  class VoidFx : public FxBinder {
    public:
      //! Defines a Calculator function that takes no arguments.
      typedef void (Calculator::*calcOp)();
       
      VoidFx(const QString &name, calcOp function,
             InlineCalculator *calculator);
      virtual ~VoidFx();
      void dispatch();
 
    private:
      calcOp  m_func;           //!< The Calculator operator that takes no parameters.
      InlineCalculator *m_calc; //!< The Calculator used to evaluate this function.
  };

  // this is a global method, outside of all classes.
  double floatModulusOperator(double a, double b);
 
} // Namespace Isis

#endif
