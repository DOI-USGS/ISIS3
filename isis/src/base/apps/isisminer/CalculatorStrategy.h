#ifndef CalculatorStrategy_h
#define CalculatorStrategy_h
/**
 * @file                                                                  
 * $Revision: 6513 $ 
 * $Date: 2016-01-14 16:04:44 -0700 (Thu, 14 Jan 2016) $ 
 * $Id: CalculatorStrategy.h 6513 2016-01-14 23:04:44Z kbecker@GS.DOI.NET $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

// parent class
#include "Strategy.h"
// contains CalculatorVariablePool parent 
#include "InlineCalculator.h"

// Qt library
#include <QString>
#include <QSharedPointer>
#include <QVector>

// SharedResource typedef
#include "PvlFlatMap.h"
#include "Resource.h"

namespace Isis {

  class PvlObject;
  struct Equation;

  /**
   * @brief CalculatorStrategy - provides inline calculations
   *
   * This strategy provides user with the ability to write new keywords to
   * each Resource based on an equation with keyword names as variables.
   *
   * Here is an example of a Calculator Strategy object definition:
   *
   * @code
   * Object = Strategy
   *   Name = Trigonometry
   *   Type = Calculator
   *   Description = "Calculate trigonometric functions of angle."
   *   Group = Initializers
   *     Sine    = 0
   *     Cosine  = 0
   *     Tangent = 0
   *   EndGroup
   *   Group = Equations
   *     Sine    = "sin(angle)"
   *     Cosine  = "cos(angle)"
   *     Tangent = "tan(angle)"
   *   EndGroup
   * EndObject
   * @endcode
   *  
   * @author 2012-07-15 Kris Becker
   *
   * @internal
   *   @history 2012-07-15 Kris Becker - Original version.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-03-24 Jeffrey Covington - Documented class and methods.
   *   @history 2015-03-31 Jeffrey Covington - Added the Equations group to support multiple
   *                                           equations per Strategy.
   *   @history 2015-05-08 Kris Becker - Modify constructor to take a global
   *                          resources list; modified apply() method to accept
   *                          a global resource list.
   *   @history 2015-06-03 Kris Becker - Modified the calculate() method to use
   *                          a list of resources as the variable pool to
   *                          support use of global parameters. This also
   *                          required changes to ResourceCalculatorVariablePool
   *                          to accept a ResourceList in addition to a
   *                          Resource.
   *   @history 2015-10-12 Kris Becker - Enhanced the initializer options to i
   *                          include argument subsititution.
   *  
   *              
   */
  class CalculatorStrategy : public Strategy {
  
    public:
      CalculatorStrategy();
      CalculatorStrategy(const PvlObject &definition, const ResourceList &globals);
      virtual ~CalculatorStrategy();
  
      virtual int apply(SharedResource &resource, const ResourceList &globals);

      double  result() const;
  
    protected:
      int    initialize(SharedResource &resource, const ResourceList &globals);
      double calculate(SharedResource &resource);
      double calculate(ResourceList &resources);
  
      int    initialize(PvlFlatMap &pvl);
      double calculate(PvlFlatMap &pvl);
  
    private:
      QList<Equation>                   m_equations;    //!< Stores the equations
      QSharedPointer<PvlFlatMap>        m_initializers; //!< Stores the initializers
      QStringList                       m_initArgs;     //!< Initializer arguments
      QSharedPointer<InlineCalculator>  m_calculator;   /**!< The current calculator to use in
                                                              calculations */
      double                            m_result;       //!< The result of the calculation
  };


  /**
   * @brief Provides Resource wrapper for variables to Calculator class 
   *  
   * This small class provides the interface to the InlineCalculator class to 
   * lookup and provide variables in equations from Resources. You can choose to 
   * provide only one resource or a resource list. 
   * 
   * @author 2013-02-02 Kris Becker 
   *  
   * @internal 
   *   @history 2015-06-03 Kris Becker - Modified to accept a ResourceList
   *                          in addition to a Resource. Now traverses the list
   *                          of Resources to find the variable.
   */
  class ResourceCalculatorVariablePool : public CalculatorVariablePool {
    public:
      ResourceCalculatorVariablePool(SharedResource &resource);
      ResourceCalculatorVariablePool(ResourceList &resources);
      virtual ~ResourceCalculatorVariablePool(); 
      virtual bool exists(const QString &variable) const;
      virtual QVector<double> value(const QString &variable, 
                                    const int &index = 0) const;
    private:
      ResourceList   m_resources; //!< A reference to the resource list
  };


  /**
   * @brief Provides PvlFlatMap wrapper for variables to Calculator class 
   *  
   * This small class provides the interface to the InlineCalculator class to 
   * lookup and provide variables in equations from Pvl sources.
   * 
   * @author 2013-02-19 Kris Becker 
   * @internal 
   *   @history 2013-02-19 Kris Becker - Original version.
   */
  class PvlFlatMapCalculatorVariablePool : public CalculatorVariablePool {
    public:
      PvlFlatMapCalculatorVariablePool(PvlFlatMap &pvl);
      virtual ~PvlFlatMapCalculatorVariablePool();
      virtual bool exists(const QString &variable) const;
      virtual QVector<double> value(const QString &variable, 
                                    const int &index = 0) const;

    private:
      PvlFlatMap &m_pvl; //!< 
  };

  /**
   * Stores relevant information about each equation.
   */
  struct Equation {
    QString equation;                            //!< Stores the string of the equation
    QString store;                               /**!< The keyword to store the result of the
                                                       equation in */
    QSharedPointer<InlineCalculator> calculator; //!< The calculator initialized from the equation 
  };
} // Namespace Isis

#endif
