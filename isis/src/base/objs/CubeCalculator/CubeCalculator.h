/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2010/04/08 15:03:37 $
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

// Calculator.h
#ifndef CUBE_CALCULATOR_H_
#define CUBE_CALCULATOR_H_

#include "Calculator.h"
#include "Cube.h"

template<class T> class QVector;

namespace Isis {
  class DataValue;

  /**
   * @brief Calculator for arrays
   *
   * This class is a RPN calculator on cubes. The base Calculator class
   *   is used in conjunction with methods to retrieve data from a cube
   *   and perform calculations.
   *
   * @ingroup Math
   *
   * @author 2008-03-26 Steven Lambright
   *
   * @internal
   *  @history 2008-05-12 Steven Lambright - Removed references to CubeInfo
   *  @history 2008-06-18 Steven Lambright - Fixed documentation
   *  @history 2009-03-03 Steven Lambright - Added missing secant method call
   *  @history 2010-02-23 Steven Lambright - Added min2,max2
   *  @history 2010-04-08 Steven Lambright - Replaced min2,max2 with proper
   *    implementations of cubemin, cubemax, linemin, linemax, min and max.
   *    Added support for getting statistics of a cube for constants.
   */
  class CubeCalculator : Calculator {
    public:
      CubeCalculator();

      /**
       * This method completely resets the calculator. The prepared
       *   calculations will be erased when this is called.
       */
      void Clear();

      void PrepareCalculations(iString equation,
                               QVector<Cube *> &inCubes,
                               Cube *outCube);

      QVector<double> RunCalculations(QVector<Buffer *> &cubeData,
                                      int line, int band);

    private:
      /**
       * This is used to define
       *   the overall action to perform in
       *   RunCalculations(..).
       */
      enum calculations {
        //! The calculation requires calling one of the methods
        callNextMethod,
        //! The calculation requires input data
        pushNextData
      };

      void AddMethodCall(void (Calculator::*method)(void));

      int LastPushToCubeStats(QVector<Cube *> &inCubes);

      /**
       * This is what RunCalculations(...) will loop over.
       *   The action to perform (push data or execute calculation)
       *   is defined in this vector.
       */
      QVector< calculations > *p_calculations;

      /**
       * This stores the addresses to the methods RunCalculations(...)
       * will call
       */
      QVector< void (Calculator:: *)(void) > *p_methods;

      /**
       * This stores the addressed to the methods RunCalculations(...)
       * will push (constants), along with placeholders for simplicity to
       * keep synchronized with the data definitions.
       */
      QVector< QVector<double> > *p_data;

      /**
       * This defines what kind of data RunCalculations(...) will push
       * onto the calculator. Constants will be taken from p_data, which
       * is synchronized (index-wise) with this vector.
       */
      QVector< DataValue > *p_dataDefinitions;

      QVector<Statistics *> *p_cubeStats;

      int p_outputSamples;
  };

  class DataValue {
    public:
      /**
       * This is used to tell what kind of data to
       *   push onto the RPN calculator.
       */
      enum dataValueType {
        constant, //!< a single constant value
        sample, //!< current sample number
        line, //!< current line number
        band, //!< current band number
        cubeData //!< a brick of cube data
      };

      DataValue() {
        p_type = (dataValueType) - 1;
        p_cubeIndex = -1;
        p_constantValue = 0.0;
      }

      DataValue(dataValueType type) {
        p_type = type;
        p_constantValue = 0.0;
        p_cubeIndex = -1;
      }

      DataValue(dataValueType type, int cubeIndex) {
        p_type = type;
        p_constantValue = 0.0;

        if(type == cubeData) {
          p_cubeIndex = cubeIndex;
        }
      }

      DataValue(dataValueType type, double value) {
        p_type = type;
        p_cubeIndex = -1;

        if(type == constant) {
          p_constantValue = value;
        }
      }

      dataValueType getType() {
        return p_type;
      }

      int getCubeIndex() {
        return p_cubeIndex;
      }

      double getContant() {
        return p_constantValue;
      }

    private:
      int p_cubeIndex;
      double p_constantValue;

      dataValueType p_type;
  };
}
#endif
