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
  class CameraBuffers;

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
   *  @history 2010-04-08 Steven Lambright - Replaced min2,max
   *                          with proper implementations of
   *                          cubemin, cubemax, linemin, linemax,
   *                          min and max. Added support for
   *                          getting statistics of a cube for
   *                          constants.
   *  @history 2012-02-02 Jeff Anderson - Added support for camera
   *                          parameters such as phase, incidence,
   *                          etc
   *  @history 2012-02-09 Jeff Anderson - Modified to conform to
   *                          ISIS coding standards
   */
  class CubeCalculator : Calculator {
    public:
      CubeCalculator();

      /**
       * This method completely resets the calculator. The prepared
       *   calculations will be erased when this is called.
       */
      void Clear();

      void prepareCalculations(IString equation,
                               QVector<Cube *> &inCubes,
                               Cube *outCube);

      QVector<double> runCalculations(QVector<Buffer *> &cubeData,
                                      int line, int band);

    private:
      /**
       * This is used to define
       *   the overall action to perform in
       *   RunCalculations(..).
       */
      enum Calculations {
        //! The calculation requires calling one of the methods
        CallNextMethod,
        //! The calculation requires input data
        PushNextData
      };

      void addMethodCall(void (Calculator::*method)(void));

      int lastPushToCubeStats(QVector<Cube *> &inCubes);

      int lastPushToCubeCameras(QVector<Cube *> &inCubes);

      /**
       * This is what RunCalculations(...) will loop over.
       *   The action to perform (push data or execute calculation)
       *   is defined in this vector.
       */
      QVector<Calculations> *m_calculations;

      /**
       * This stores the addresses to the methods RunCalculations(...)
       * will call
       */
      QVector<void (Calculator:: *)(void)> *m_methods;

      /**
       * This stores the addressed to the methods RunCalculations(...)
       * will push (constants), along with placeholders for simplicity to
       * keep synchronized with the data definitions.
       */
      QVector< QVector<double> > *m_data;

      /**
       * This defines what kind of data RunCalculations(...) will push
       * onto the calculator. Constants will be taken from p_data, which
       * is synchronized (index-wise) with this vector.
       */
      QVector<DataValue> *m_dataDefinitions;

      QVector<Statistics *> *m_cubeStats;

      /**
       * This two are used for keeping cameras and camera buffers 
       * internalized for quick computations
       */
      QVector<Camera *> *m_cubeCameras;

      QVector<CameraBuffers *> *m_cameraBuffers;

      int m_outputSamples;
  };

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class DataValue {
    public:
      /**
       * This is used to tell what kind of data to
       *   push onto the RPN calculator.
       */
      enum DataValueType {
        Constant, //!< a single constant value
        Sample, //!< current sample number
        Line, //!< current line number
        Band, //!< current band number
        CubeData, //!< a brick of cube data
        InaData, //!< incidence camera data
        EmaData, //!< emission camera data
        PhaData, //!< phase camera data
        LatData, //!< Latitude camera data
        LonData, //!< Longitude camera data
        ResData, //!< Pixel resolution camera data
        RadiusData, //!< DEM radius
        InalData, //!< local incidence camera data
        EmalData, //!< local emission camera data
        PhalData //!< local phase camera data
      };

      DataValue() {
        m_type = (DataValueType) - 1;
        m_cubeIndex = -1;
        m_constantValue = 0.0;
      }

      DataValue(DataValueType type) {
        m_type = type;
        m_constantValue = 0.0;
        m_cubeIndex = -1;
      }

      DataValue(DataValueType type, int cubeIndex) {
        m_type = type;
        m_constantValue = 0.0;
        m_cubeIndex = cubeIndex;
      }

      DataValue(DataValueType type, double value) {
        m_type = type;
        m_cubeIndex = -1;

        if(type == Constant) {
          m_constantValue = value;
        }
      }

      DataValueType getType() {
        return m_type;
      }

      int getCubeIndex() {
        return m_cubeIndex;
      }

      double getConstant() {
        return m_constantValue;
      }

    private:
      int m_cubeIndex;
      double m_constantValue;

      DataValueType m_type;
  };

  /**
   * @author 2012-02-02 Jeff Anderson
   *
   * @internal
   */
  class CameraBuffers {
    public:
      CameraBuffers(Camera *camera);
      ~CameraBuffers();

       void enablePhaBuffer();
       void enableInaBuffer(); 
       void enableEmaBuffer(); 
       void enableLatBuffer(); 
       void enableLonBuffer(); 
       void enableResBuffer(); 
       void enableRadiusBuffer(); 
       void enablePhalBuffer();
       void enableInalBuffer(); 
       void enableEmalBuffer(); 

       QVector<double> *getPhaBuffer (int currentLine, int ns);
       QVector<double> *getInaBuffer (int currentLine, int ns);
       QVector<double> *getEmaBuffer (int currentLine, int ns);
       QVector<double> *getLatBuffer (int currentLine, int ns);
       QVector<double> *getLonBuffer (int currentLine, int ns);
       QVector<double> *getResBuffer (int currentLine, int ns);
       QVector<double> *getRadiusBuffer (int currentLine, int ns);
       QVector<double> *getPhalBuffer (int currentLine, int ns);
       QVector<double> *getInalBuffer (int currentLine, int ns);
       QVector<double> *getEmalBuffer (int currentLine, int ns);


    private:
      void loadBuffers (int currentLine, int ns);

      Camera *m_camera;
      int m_lastLine;

      QVector<double> *m_phaBuffer;
      QVector<double> *m_inaBuffer;
      QVector<double> *m_emaBuffer;
      QVector<double> *m_phalBuffer;
      QVector<double> *m_inalBuffer;
      QVector<double> *m_emalBuffer;
      QVector<double> *m_resBuffer;
      QVector<double> *m_latBuffer;
      QVector<double> *m_lonBuffer;
      QVector<double> *m_radiusBuffer;
  };
}
#endif
