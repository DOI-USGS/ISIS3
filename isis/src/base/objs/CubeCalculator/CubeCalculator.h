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

class QString;
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
   *  @history 2015-01-30 Ian Humphrey - Removed unused variable m_data. Deallocated
   *                          unfreed dynamic memory. Added destructor. Fixes #2082.
   *  @history 2016-10-13 Ian Humphrey - Updated to correctly calculate center camera angles for
   *                          band-depedent images. Integrated Moses Milazzo's (moses@usgs.gov)
   *                          changes for correctly calculating camera angles for band-dependent
   *                          images. Quick documentation and coding standards review (moved
   *                          inline implementations to cpp). Fixes #1301.
   */
  class CubeCalculator : Calculator {
    public:
      CubeCalculator();
      ~CubeCalculator();

      /**
       * This method completely resets the calculator. The prepared
       *   calculations will be erased when this is called.
       */
      void Clear();

      void prepareCalculations(QString equation,
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

      //! This stores the addresses to the methods RunCalculations(...) will call.
      QVector<void (Calculator:: *)(void)> *m_methods;

      //! This defines what kind of data RunCalculations(...) will push onto the calculator.
      QVector<DataValue> *m_dataDefinitions;

      //! Stores the cube statistics for the input cubes. 
      QVector<Statistics *> *m_cubeStats;

      /**
       * Stores the cameras for the input cubes. This is synchronized with m_cameraBuffers
       * so that the camera buffers are loaded with data from the appropriate camera. 
       */
      QVector<Camera *> *m_cubeCameras;

      /**
       * Stores the camera buffers that are enabled for camera related calculations.
       * This is used for quickly computing camera related information.
       */
      QVector<CameraBuffers *> *m_cameraBuffers;

      int m_outputSamples; //!< Number of samples in the output cube.
  };


  /**
   * This class is used to define what kind of data is being pushed onto the cube calculator
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class DataValue {
    public:
      //! This is used to tell what kind of data to push onto the RPN calculator.
      enum DataValueType {
        Constant, //!< A single constant value.
        Sample, //!< Current sample number.
        Line, //!< Current line number.
        Band, //!< Current band number.
        CubeData, //!< A brick of cube data.
        InaData, //!< Incidence camera data.
        EmaData, //!< Emission camera data.
        PhaData, //!< Phase camera data.
        LatData, //!< Latitude camera data.
        LonData, //!< Longitude camera data.
        ResData, //!< Pixel resolution camera data.
        RadiusData, //!< DEM radius.
        InalData, //!< Local incidence camera data.
        EmalData, //!< Local emission camera data.
        PhalData, //!< Local phase camera data.
        InacData, //!< Center incidence camera data.
        EmacData, //!< Center emission camera data.
        PhacData  //!< Center phase camera data.
      };

      DataValue();
      DataValue(DataValueType type);
      DataValue(DataValueType type, int cubeIndex);
      DataValue(DataValueType type, double value);

      DataValueType type();
      int cubeIndex();
      double constant();

    private:
      int m_cubeIndex;        //!< The index of the associated cube
      double m_constantValue; //!< Stored constant value

      DataValueType m_type;   //!< Type of data stored.
  };


  /**
   * This class is used to manage buffers for calculating camera related information, such
   * as angles, radii, and resolution. It uses internal buffers for each of the camera related
   * operators that CubeCalculator recognizes as valid tokens. Each of the enableBuffer methods
   * can be used to dynamically allocate buffers for the camera operators that are being pushed
   * onto the CubeCalculator. Buffers can be loaded with appropriate camera data using the 
   * accessor methods.
   *
   * Note that memory is not allocated for buffers that are not enabled.
   *
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
       void enablePhacBuffer();
       void enableInacBuffer();
       void enableEmacBuffer();

       // Accessors
       QVector<double> *phaBuffer(int currentLine, int ns, int currentBand);
       QVector<double> *inaBuffer(int currentLine, int ns, int currentBand);
       QVector<double> *emaBuffer(int currentLine, int ns, int currentBand);
       QVector<double> *latBuffer(int currentLine, int ns, int currentBand);
       QVector<double> *lonBuffer(int currentLine, int ns, int currentBand);
       QVector<double> *resBuffer(int currentLine, int ns, int currentBand);
       QVector<double> *radiusBuffer(int currentLine, int ns, int currentBand);
       QVector<double> *phalBuffer(int currentLine, int ns, int currentBand);
       QVector<double> *inalBuffer(int currentLine, int ns, int currentBand);
       QVector<double> *emalBuffer(int currentLine, int ns, int currentBand);
       QVector<double> *phacBuffer(int currentLine, int ns, int currentBand);
       QVector<double> *inacBuffer(int currentLine, int ns, int currentBand);
       QVector<double> *emacBuffer(int currentLine, int ns, int currentBand);


    private:
      void loadBuffers(int currentLine, int ns, int currentBand);

      Camera *m_camera; //!< Camera to obtain camera-related information from.
      int m_lastLine; //!< The number of the last line loaded into the enabled camera buffers.

      QVector<double> *m_phaBuffer;    //!< Phase angle buffer.
      QVector<double> *m_inaBuffer;    //!< Incidence angle buffer.
      QVector<double> *m_emaBuffer;    //!< Emission angle buffer.
      QVector<double> *m_phalBuffer;   //!< Local phase angle buffer.
      QVector<double> *m_inalBuffer;   //!< Local incidence angle buffer.
      QVector<double> *m_emalBuffer;   //!< Local emission angle buffer.
      QVector<double> *m_phacBuffer;   //!< Center phase angle buffer.
      QVector<double> *m_inacBuffer;   //!< Center incidence angle buffer.
      QVector<double> *m_emacBuffer;   //!< Center emission angle buffer.
      QVector<double> *m_resBuffer;    //!< Resolution buffer.
      QVector<double> *m_latBuffer;    //!< Latitude buffer.
      QVector<double> *m_lonBuffer;    //!< Longitude buffer.
      QVector<double> *m_radiusBuffer; //!< Radius buffer.
  };
}
#endif
