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

#include <QVector>

#include "CubeCalculator.h"
#include "IString.h"
#include "Statistics.h"
#include "Camera.h"
#include "Distance.h"
#include "Angle.h"

using namespace std;

namespace Isis {

  //! Constructor
  CubeCalculator::CubeCalculator() {
    m_calculations    = NULL;
    m_methods         = NULL;
    m_data            = NULL;
    m_dataDefinitions = NULL;
    m_cubeStats       = NULL;
    m_cubeCameras     = NULL;
    m_cameraBuffers   = NULL;

    m_calculations    = new QVector<Calculations>();
    m_methods         = new QVector<void (Calculator:: *)(void)>();
    m_data            = new QVector<QVector<double> >();
    m_dataDefinitions = new QVector<DataValue>();
    m_cubeStats       = new QVector<Statistics *>();
    m_cubeCameras     = new QVector<Camera *>();
    m_cameraBuffers   = new QVector<CameraBuffers *>();

    m_outputSamples = 0;
  }

  void CubeCalculator::Clear() {
    Calculator::Clear();

    if (m_calculations) {
      m_calculations->clear();
    }

    if (m_methods) {
      m_methods->clear();
    }

    if (m_data) {
      m_data->clear();
    }

    if (m_dataDefinitions) {
      m_dataDefinitions->clear();
    }

    if (m_cubeStats) {
      m_cubeStats->clear();
    }

    if (m_cubeCameras) {
      m_cubeCameras->clear();
    }

    if (m_cameraBuffers) {
      m_cameraBuffers->clear();
    }
  }

  /**
   * This method will execute the calculations built up when PrepareCalculations was called.
   *
   * @param cubeData The input cubes' data
   * @param curLine The current line in the output cube
   * @param curBand The current band in the output cube
   *
   * @return std::vector<double> The results of the calculations (with Isis Special Pixels)
   *
   * @throws iException::Math
   */
  QVector<double> CubeCalculator::runCalculations(QVector<Buffer *> &cubeData, int curLine, int curBand) {
    // For now we'll only process a single line in this method for our results. In order
    //    to do more powerful indexing, passing a list of cubes and the output cube will
    //    be necessary.
    int methodIndex = 0;
    int dataIndex = 0;
    for (int currentCalculation = 0; currentCalculation < m_calculations->size();
        currentCalculation++) {
      if ((*m_calculations)[currentCalculation] == CallNextMethod) {
        void (Calculator::*aMethod)() = (*m_methods)[methodIndex];
        (this->*aMethod)();
        methodIndex ++;
      }
      else {
        DataValue &data = (*m_dataDefinitions)[dataIndex];
        if (data.getType() == DataValue::Constant) {
          Push(data.getConstant());
        }
        else if (data.getType() == DataValue::Band) {
          Push(curBand);
        }
        else if (data.getType() == DataValue::Line) {
          Push(curLine);
        }
        else if (data.getType() == DataValue::Sample) {
          QVector<double> samples;
          samples.resize(m_outputSamples);

          for (int i = 0; i < m_outputSamples; i++) {
            samples[i] = i + 1;
          }

          Push(samples);
        }
        else if (data.getType() == DataValue::CubeData) {
          Push(*cubeData[data.getCubeIndex()]);
        }
        else if (data.getType() == DataValue::InaData) {
          Push(*((*m_cameraBuffers)[data.getCubeIndex()]->getInaBuffer(curLine,m_outputSamples)));
        }
        else if (data.getType() == DataValue::EmaData) {
          Push(*((*m_cameraBuffers)[data.getCubeIndex()]->getEmaBuffer(curLine,m_outputSamples)));
        }
        else if (data.getType() == DataValue::PhaData) {
          Push(*((*m_cameraBuffers)[data.getCubeIndex()]->getPhaBuffer(curLine,m_outputSamples)));
        }
        else if (data.getType() == DataValue::InalData) {
          Push(*((*m_cameraBuffers)[data.getCubeIndex()]->getInalBuffer(curLine,m_outputSamples)));
        }
        else if (data.getType() == DataValue::EmalData) {
          Push(*((*m_cameraBuffers)[data.getCubeIndex()]->getEmalBuffer(curLine,m_outputSamples)));
        }
        else if (data.getType() == DataValue::PhalData) {
          Push(*((*m_cameraBuffers)[data.getCubeIndex()]->getPhalBuffer(curLine,m_outputSamples)));
        }
        else if (data.getType() == DataValue::LatData) {
          Push(*((*m_cameraBuffers)[data.getCubeIndex()]->getLatBuffer(curLine,m_outputSamples)));
        }
        else if (data.getType() == DataValue::LonData) {
          Push(*((*m_cameraBuffers)[data.getCubeIndex()]->getLonBuffer(curLine,m_outputSamples)));
        }
        else if (data.getType() == DataValue::ResData) {
          Push(*((*m_cameraBuffers)[data.getCubeIndex()]->getResBuffer(curLine,m_outputSamples)));
        }
        else if (data.getType() == DataValue::RadiusData) {
          Push(*((*m_cameraBuffers)[data.getCubeIndex()]->getRadiusBuffer(curLine,m_outputSamples)));
        }
        else {
        }

        dataIndex ++;
      }
    }

    if (StackSize() != 1) {
      string msg = "Too many operands in the equation.";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    return Pop(true);
  }


  /**
   * This method builds a list of actions to perform based on the postfix expression.
   *   Error checking is done using the inCubeInfos, and the outCubeInfo is necessary
   *   to tell the dimensions of the output cube. Call this method before calling
   *   RunCalculations(). This method will also erase all calculator history before building
   *   up a new set of calculations to run.
   *
   * @param equation The equation in postfix notation
   * @param inCubes The input cubes
   * @param outCube The output cube
   */
  void CubeCalculator::prepareCalculations(IString equation,
      QVector<Cube *> &inCubes,
      Cube *outCube) {
    Clear();

    m_outputSamples = outCube->getSampleCount();

    IString eq = equation;
    while (eq != "") {
      IString token = eq.Token(" ");

      // Step through every part of the postfix equation and set up the appropriate
      // action list based on the current token. Attempting to order if-else conditions
      // in terms of what would probably be encountered more often.

      // Scalars
      if (isdigit(token[0]) || token[0] == '.') {
        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(DataValue(DataValue::Constant,
                                               token.ToDouble()));
      }
      // File, e.g. F1 = first file in list. Must come after any functions starting with 'f' that
      //   is not a cube.
      else if (token[0] == 'f') {
        IString tok(token.substr(1));
        int file = tok.ToInteger() - 1;
        if (file < 0 || file >= (int)inCubes.size()) {
          std::string msg = "Invalid file number [" + tok + "]";
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }

        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(DataValue(DataValue::CubeData, file));
      }
      else if (token == "band") {
        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(DataValue(DataValue::Band));
      }
      else if (token == "line") {
        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(DataValue(DataValue::Line));
      }
      else if (token == "sample") {
        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(DataValue(DataValue::Sample));
      }
      // Addition
      else if (token == "+") {
        addMethodCall(&Calculator::Add);
      }

      // Subtraction
      else if (token == "-") {
        addMethodCall(&Calculator::Subtract);
      }

      // Multiplication
      else if (token == "*") {
        addMethodCall(&Calculator::Multiply);
      }

      // Division
      else if (token == "/") {
        addMethodCall(&Calculator::Divide);
      }

      // Modulus
      else if (token == "%") {
        addMethodCall(&Calculator::Modulus);
      }

      // Exponent
      else if (token == "^") {
        addMethodCall(&Calculator::Exponent);
      }

      // Negative
      else if (token == "--") {
        addMethodCall(&Calculator::Negative);
      }

      // Negative
      else if (token == "neg") {
        addMethodCall(&Calculator::Negative);
      }

      // Left shift
      else if (token == "<<") {
        addMethodCall(&Calculator::LeftShift);
      }

      // Right shift
      else if (token == ">>") {
        addMethodCall(&Calculator::RightShift);
      }

      // Maximum In The Line
      else if (token == "linemax") {
        addMethodCall(&Calculator::MaximumLine);
      }

      // Maximum Pixel on a per-pixel basis
      else if (token == "max") {
        addMethodCall(&Calculator::MaximumPixel);
      }

      // Minimum In The Line
      else if (token == "linemin") {
        addMethodCall(&Calculator::MinimumLine);
      }

      // Minimum Pixel on a per-pixel basis
      else if (token == "min") {
        addMethodCall(&Calculator::MinimumPixel);
      }

      // Absolute value
      else if (token == "abs") {
        addMethodCall(&Calculator::AbsoluteValue);
      }

      // Square root
      else if (token == "sqrt") {
        addMethodCall(&Calculator::SquareRoot);
      }

      // Natural Log
      else if (token == "log" || token == "ln") {
        addMethodCall(&Calculator::Log);
      }

      // Log base 10
      else if (token == "log10") {
        addMethodCall(&Calculator::Log10);
      }

      // Pi
      else if (token == "pi") {
        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(
          DataValue(DataValue::Constant, PI)
        );
      }

      // e
      else if (token == "e") {
        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(
          DataValue(DataValue::Constant, E)
        );
      }

      else if (token == "rads") {
        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(
          DataValue(DataValue::Constant, PI / 180.0)
        );

        addMethodCall(&Calculator::Multiply);
      }

      else if (token == "degs") {
        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(
          DataValue(DataValue::Constant, 180.0 / PI)
        );
        addMethodCall(&Calculator::Multiply);
      }

      // Sine
      else if (token == "sin") {
        addMethodCall(&Calculator::Sine);
      }

      // Cosine
      else if (token == "cos") {
        addMethodCall(&Calculator::Cosine);
      }

      // Tangent
      else if (token == "tan") {
        addMethodCall(&Calculator::Tangent);
      }

      // Secant
      else if (token == "sec") {
        addMethodCall(&Calculator::Secant);
      }

      // Cosecant
      else if (token == "csc") {
        addMethodCall(&Calculator::Cosecant);
      }

      // Cotangent
      else if (token == "cot") {
        addMethodCall(&Calculator::Cotangent);
      }

      // Arcsin
      else if (token == "asin") {
        addMethodCall(&Calculator::Arcsine);
      }

      // Arccos
      else if (token == "acos") {
        addMethodCall(&Calculator::Arccosine);
      }

      // Arctan
      else if (token == "atan") {
        addMethodCall(&Calculator::Arctangent);
      }

      // Arctan2
      else if (token == "atan2") {
        addMethodCall(&Calculator::Arctangent2);
      }

      // SineH
      else if (token == "sinh") {
        addMethodCall(&Calculator::SineH);
      }

      // CosH
      else if (token == "cosh") {
        addMethodCall(&Calculator::CosineH);
      }

      // TanH
      else if (token == "tanh") {
        addMethodCall(&Calculator::TangentH);
      }

      // Less than
      else if (token == "<") {
        addMethodCall(&Calculator::LessThan);
      }

      // Greater than
      else if (token == ">") {
        addMethodCall(&Calculator::GreaterThan);
      }

      // Less than or equal
      else if (token == "<=") {
        addMethodCall(&Calculator::LessThanOrEqual);
      }

      // Greater than or equal
      else if (token == ">=") {
        addMethodCall(&Calculator::GreaterThanOrEqual);
      }

      // Equal
      else if (token == "==") {
        addMethodCall(&Calculator::Equal);
      }

      // Not equal
      else if (token == "!=") {
        addMethodCall(&Calculator::NotEqual);
      }

      // Maximum in a cube
      else if (token == "cubemax") {
        int cubeIndex = lastPushToCubeStats(inCubes);

        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(
          DataValue(DataValue::Constant, (*m_cubeStats)[cubeIndex]->Maximum())
        );
        //TODO: Test for NULL Maximum
      }

      // Maximum in a cube
      else if (token == "cubemin") {
        int cubeIndex = lastPushToCubeStats(inCubes);

        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(
          DataValue(DataValue::Constant, (*m_cubeStats)[cubeIndex]->Minimum())
        );
        //TODO: Test for NULL Minimum
      }

      // Average of a cube
      else if (token == "cubeavg") {
        int cubeIndex = lastPushToCubeStats(inCubes);

        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(
          DataValue(DataValue::Constant, (*m_cubeStats)[cubeIndex]->Average())
        );
        //TODO: Test for NULL Average
      }

      // Standard deviation of a cube
      else if (token == "cubestd") {
        int cubeIndex = lastPushToCubeStats(inCubes);

        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(
          DataValue(DataValue::Constant, (*m_cubeStats)[cubeIndex]->StandardDeviation())
        );
        //TODO: Test for NULL standard deviation
      }

      // Center phase in a cube
      else if ((token == "phac") || (token == "inac") || (token == "emac")) {
        int cubeIndex = lastPushToCubeCameras(inCubes);

        double centerLine = inCubes[cubeIndex]->getLineCount() / 2.0 + 0.5;
        double centerSamp = inCubes[cubeIndex]->getSampleCount() / 2.0 + 0.5;
        Camera *cam = (*m_cubeCameras)[cubeIndex];

        if (cam->SetImage(centerSamp, centerLine)) {
          m_calculations->push_back(PushNextData);
          if (token == "inac") {
            m_dataDefinitions->push_back(DataValue(DataValue::Constant, cam->IncidenceAngle()));
          }
          else if (token == "emac") {
            m_dataDefinitions->push_back(DataValue(DataValue::Constant, cam->EmissionAngle()));
          }
          else {
            m_dataDefinitions->push_back(DataValue(DataValue::Constant, cam->PhaseAngle()));
          }
        }
        else {
          string msg = "Unable to compute illumination angles at image center for operator [";
          msg += token + "] using input file [f" + IString(cubeIndex + 1) + "]";
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }
      }

      // Incidence on the ellipsoid
      else if (token == "ina") {
        int cubeIndex = lastPushToCubeCameras(inCubes);
        (*m_cameraBuffers)[cubeIndex]->enableInaBuffer();

        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(DataValue(DataValue::InaData, cubeIndex));
      }

      // Emission on the ellipsoid
      else if (token == "ema") {
        int cubeIndex = lastPushToCubeCameras(inCubes);
        (*m_cameraBuffers)[cubeIndex]->enableEmaBuffer();

        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(DataValue(DataValue::EmaData, cubeIndex));
      }

      // Phase on the ellipsoid
      else if (token == "pha") {
        int cubeIndex = lastPushToCubeCameras(inCubes);
        (*m_cameraBuffers)[cubeIndex]->enablePhaBuffer();

        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(DataValue(DataValue::PhaData, cubeIndex));
      }

      // Incidence on the DTM
      else if (token == "inal") {
        int cubeIndex = lastPushToCubeCameras(inCubes);
        (*m_cameraBuffers)[cubeIndex]->enableInalBuffer();

        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(DataValue(DataValue::InalData, cubeIndex));
      }

      // Emission on the DTM
      else if (token == "emal") {
        int cubeIndex = lastPushToCubeCameras(inCubes);
        (*m_cameraBuffers)[cubeIndex]->enableEmalBuffer();

        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(DataValue(DataValue::EmalData, cubeIndex));
      }

      // Phase on the ellipsoid
      else if (token == "phal") {
        int cubeIndex = lastPushToCubeCameras(inCubes);
        (*m_cameraBuffers)[cubeIndex]->enablePhalBuffer();

        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(DataValue(DataValue::PhalData, cubeIndex));
      }

      // Latitude
      else if (token == "lat") {
        int cubeIndex = lastPushToCubeCameras(inCubes);
        (*m_cameraBuffers)[cubeIndex]->enableLatBuffer();

        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(DataValue(DataValue::LatData, cubeIndex));
      }

      // Longitude
      else if (token == "lon") {
        int cubeIndex = lastPushToCubeCameras(inCubes);
        (*m_cameraBuffers)[cubeIndex]->enableLonBuffer();

        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(DataValue(DataValue::LonData, cubeIndex));
      }

      // Pixel resolution
      else if (token == "res") {
        int cubeIndex = lastPushToCubeCameras(inCubes);
        (*m_cameraBuffers)[cubeIndex]->enableResBuffer();

        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(DataValue(DataValue::ResData, cubeIndex));
      }

      // Local Radius
      else if (token == "radius") {
        int cubeIndex = lastPushToCubeCameras(inCubes);
        (*m_cameraBuffers)[cubeIndex]->enableRadiusBuffer();

        m_calculations->push_back(PushNextData);
        m_dataDefinitions->push_back(DataValue(DataValue::RadiusData, cubeIndex));
      }

      // Ignore empty token
      else if (token == "") {
      }

      else {
        string msg = "Unidentified operator [";
        msg += token + "]";
        throw IException(IException::Unknown, msg, _FILEINFO_);
      }
    } // while loop
  }

  int CubeCalculator::lastPushToCubeStats(QVector<Cube *> &inCubes) {
    if (!m_calculations->size()) {
      string msg = "Not sure which file to get statistics from";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    if ((*m_calculations)[m_calculations->size() - 1] != PushNextData) {
      string msg = "This function must not contain calculations,";
      msg += " only input cubes may be specified.";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    m_calculations->pop_back();

    // This must have data if calculations had data that equaled push data
    DataValue lastData = (*m_dataDefinitions)[m_dataDefinitions->size() - 1];

    if (lastData.getType() != DataValue::CubeData) {
      string msg = "This function must not contain constants,";
      msg += " only input cubes may be specified.";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    int cubeStatsIndex = lastData.getCubeIndex();
    m_dataDefinitions->pop_back();

    // Member variables are now cleaned up, we need to verify the stats exists

    // Make sure room exists in the vector
    while (m_cubeStats->size() < cubeStatsIndex + 1) {
      m_cubeStats->push_back(NULL);
    }

    // Now we can for sure put the stats object in the right place... put it
    //   there
    if ((*m_cubeStats)[cubeStatsIndex] == NULL) {
      (*m_cubeStats)[cubeStatsIndex] = inCubes[cubeStatsIndex]->getStatistics();
    }

    return cubeStatsIndex;
  }


  int CubeCalculator::lastPushToCubeCameras(QVector<Cube *> &inCubes) {
    if (!m_calculations->size()) {
      string msg = "Not sure which file to get cameras from";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    if ((*m_calculations)[m_calculations->size() - 1] != PushNextData) {
      string msg = "This function must not contain calculations,";
      msg += " only input cubes may be specified.";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    m_calculations->pop_back();

    // This must have data if calculations had data that equaled push data
    DataValue lastData = (*m_dataDefinitions)[m_dataDefinitions->size() - 1];

    if (lastData.getType() != DataValue::CubeData) {
      string msg = "This function must not contain constants,";
      msg += " only input cubes may be specified.";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    int cubeIndex = lastData.getCubeIndex();
    m_dataDefinitions->pop_back();

    // Member variables are now cleaned up, we need to verify the camera exists

    // Make sure room exists in the vector
    while (m_cubeCameras->size() < cubeIndex + 1) {
      m_cubeCameras->push_back(NULL);
    }

    while (m_cameraBuffers->size() < cubeIndex + 1) {
      m_cameraBuffers->push_back(NULL);
    }

    // Now we can for sure put the camera object in the right place... put it
    //   there
    if ((*m_cubeCameras)[cubeIndex] == NULL) {
      Camera *cam;
      try {
        cam = inCubes[cubeIndex]->getCamera();
        if (cam == NULL) {
          string msg = "This function requires a camera and the input cube does";
          msg += " not have one. You may need to run spiceinit";
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }
      }
      catch (IException &e) {
        string msg = "This function requires a camera and the input cube does";
        msg += " not have one. You may need to run spiceinit";
        throw IException(e, IException::Unknown, msg, _FILEINFO_);
      }

      (*m_cubeCameras)[cubeIndex] = cam;
      (*m_cameraBuffers)[cubeIndex] = new CameraBuffers(cam);
    }

    return cubeIndex;
  }


  /**
   * This is a conveinience method for PrepareCalculations(...).
   * This will cause RunCalculations(...) to execute this method in order.
   *
   * @param method The method to call, i.e. &Calculator::Multiply
   */
  void CubeCalculator::addMethodCall(void (Calculator::*method)(void)) {
    m_calculations->push_back(CallNextMethod);
    m_methods->push_back(method);
  }





  CameraBuffers::CameraBuffers(Camera *camera) {
    m_camera = camera;
    m_phaBuffer  = NULL;
    m_inaBuffer  = NULL;
    m_emaBuffer  = NULL;
    m_phalBuffer = NULL;
    m_inalBuffer = NULL;
    m_emalBuffer = NULL;
    m_resBuffer  = NULL;
    m_latBuffer  = NULL;
    m_lonBuffer  = NULL;
    m_radiusBuffer = NULL;

    m_lastLine = -1;
  }

  CameraBuffers::~CameraBuffers() {
    delete m_phaBuffer;
    delete m_inaBuffer;
    delete m_emaBuffer;
    delete m_phalBuffer;
    delete m_inalBuffer;
    delete m_emalBuffer;
    delete m_resBuffer;
    delete m_latBuffer;
    delete m_lonBuffer;
    delete m_radiusBuffer;

    m_phaBuffer  = NULL;
    m_inaBuffer  = NULL;
    m_emaBuffer  = NULL;
    m_phalBuffer = NULL;
    m_inalBuffer = NULL;
    m_emalBuffer = NULL;
    m_resBuffer  = NULL;
    m_latBuffer  = NULL;
    m_lonBuffer  = NULL;
    m_radiusBuffer = NULL;
  }

  void CameraBuffers::enablePhaBuffer() {
    if (!m_phaBuffer) m_phaBuffer = new QVector<double>;
  }

  void CameraBuffers::enableInaBuffer() {
    if (!m_inaBuffer) m_inaBuffer = new QVector<double>;
  }

  void CameraBuffers::enableEmaBuffer() {
    if (!m_emaBuffer) m_emaBuffer = new QVector<double>;
  }

  void CameraBuffers::enableLatBuffer() {
    if (!m_latBuffer) m_latBuffer = new QVector<double>;
  }

  void CameraBuffers::enableLonBuffer() {
    if (!m_lonBuffer) m_lonBuffer = new QVector<double>;
  }

  void CameraBuffers::enableResBuffer() {
    if (!m_resBuffer) m_resBuffer = new QVector<double>;
  }

  void CameraBuffers::enableRadiusBuffer() {
    if (!m_radiusBuffer) m_radiusBuffer = new QVector<double>;
  }

  void CameraBuffers::enablePhalBuffer() {
    if (!m_phalBuffer) m_phalBuffer = new QVector<double>;
  }

  void CameraBuffers::enableInalBuffer() {
    if (!m_inalBuffer) m_inalBuffer = new QVector<double>;
  }

  void CameraBuffers::enableEmalBuffer() {
    if (!m_emalBuffer) m_emalBuffer = new QVector<double>;
  }

  QVector<double> *CameraBuffers::getPhaBuffer(int currentLine, int ns) {
    loadBuffers(currentLine,ns);
    return m_phaBuffer;
  }

  QVector<double> *CameraBuffers::getInaBuffer(int currentLine, int ns) {
    loadBuffers(currentLine,ns);
    return m_inaBuffer;
  }

  QVector<double> *CameraBuffers::getEmaBuffer(int currentLine, int ns) {
    loadBuffers(currentLine,ns);
    return m_emaBuffer;
  }

  QVector<double> *CameraBuffers::getLatBuffer(int currentLine, int ns) {
    loadBuffers(currentLine,ns);
    return m_latBuffer;
  }

  QVector<double> *CameraBuffers::getLonBuffer(int currentLine, int ns) {
    loadBuffers(currentLine,ns);
    return m_lonBuffer;
  }

  QVector<double> *CameraBuffers::getResBuffer(int currentLine, int ns) {
    loadBuffers(currentLine,ns);
    return m_resBuffer;
  }

  QVector<double> *CameraBuffers::getRadiusBuffer(int currentLine, int ns) {
    loadBuffers(currentLine,ns);
    return m_radiusBuffer;
  }

  QVector<double> *CameraBuffers::getPhalBuffer(int currentLine, int ns) {
    loadBuffers(currentLine,ns);
    return m_phalBuffer;
  }

  QVector<double> *CameraBuffers::getInalBuffer(int currentLine, int ns) {
    loadBuffers(currentLine,ns);
    return m_inalBuffer;
  }

  QVector<double> *CameraBuffers::getEmalBuffer(int currentLine, int ns) {
    loadBuffers(currentLine,ns);
    return m_emalBuffer;
  }

  void CameraBuffers::loadBuffers(int currentLine, int ns) {
    if (currentLine != m_lastLine) {
      m_lastLine = currentLine;

      // Resize buffers if necessary
      if (m_phaBuffer) m_phaBuffer->resize(ns);
      if (m_inaBuffer) m_inaBuffer->resize(ns);
      if (m_emaBuffer) m_emaBuffer->resize(ns);
      if (m_latBuffer) m_latBuffer->resize(ns);
      if (m_lonBuffer) m_lonBuffer->resize(ns);
      if (m_resBuffer) m_resBuffer->resize(ns);
      if (m_radiusBuffer) m_radiusBuffer->resize(ns);
      if (m_phalBuffer) m_phalBuffer->resize(ns);
      if (m_inalBuffer) m_inalBuffer->resize(ns);
      if (m_emalBuffer) m_emalBuffer->resize(ns);

      for (int i = 0; i < ns; i++) {
        if (m_camera->SetImage(i + 1, currentLine)) {
          if (m_phaBuffer) (*m_phaBuffer)[i] = m_camera->PhaseAngle();
          if (m_inaBuffer) (*m_inaBuffer)[i] = m_camera->IncidenceAngle();
          if (m_emaBuffer) (*m_emaBuffer)[i] = m_camera->EmissionAngle();
          if (m_latBuffer) (*m_latBuffer)[i] = m_camera->UniversalLatitude();
          if (m_lonBuffer) (*m_lonBuffer)[i] = m_camera->UniversalLongitude();
          if (m_resBuffer) (*m_resBuffer)[i] = m_camera->PixelResolution();
          if (m_radiusBuffer) (*m_radiusBuffer)[i] = m_camera->LocalRadius().meters();
          if (m_phalBuffer || m_inalBuffer || m_emalBuffer) {
            Angle phal, inal, emal;
            bool okay;
            m_camera->LocalPhotometricAngles(phal, inal, emal, okay);
            if (okay) {
              if (m_phalBuffer) (*m_phalBuffer)[i] = phal.degrees();
              if (m_inalBuffer) (*m_inalBuffer)[i] = inal.degrees();
              if (m_emalBuffer) (*m_emalBuffer)[i] = emal.degrees();
            }
            else {
              if (m_phalBuffer) (*m_phalBuffer)[i] = NAN;
              if (m_inalBuffer) (*m_inalBuffer)[i] = NAN;
              if (m_emalBuffer) (*m_emalBuffer)[i] = NAN;
            }
          }
        }
        else {
          if (m_phaBuffer) (*m_phaBuffer)[i] = NAN;
          if (m_inaBuffer) (*m_inaBuffer)[i] = NAN;
          if (m_emaBuffer) (*m_emaBuffer)[i] = NAN;
          if (m_latBuffer) (*m_latBuffer)[i] = NAN;
          if (m_lonBuffer) (*m_lonBuffer)[i] = NAN;
          if (m_resBuffer) (*m_resBuffer)[i] = NAN;
          if (m_radiusBuffer) (*m_radiusBuffer)[i] = NAN;
          if (m_phalBuffer) (*m_phalBuffer)[i] = NAN;
          if (m_inalBuffer) (*m_inalBuffer)[i] = NAN;
          if (m_emalBuffer) (*m_emalBuffer)[i] = NAN;
        }
      }
    }
  }

} // End of namespace Isis

