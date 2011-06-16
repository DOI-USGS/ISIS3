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
#include "iString.h"
#include "Statistics.h"

using namespace std;

namespace Isis {

  //! Constructor
  CubeCalculator::CubeCalculator() {
    p_calculations    = NULL;
    p_methods         = NULL;
    p_data            = NULL;
    p_dataDefinitions = NULL;
    p_cubeStats       = NULL;

    p_calculations    = new QVector<calculations>();
    p_methods         = new QVector< void (Calculator:: *)(void) >();
    p_data            = new QVector< QVector<double> >();
    p_dataDefinitions = new QVector< DataValue >();
    p_cubeStats       = new QVector< Statistics * >();

    p_outputSamples = 0;
  }

  void CubeCalculator::Clear() {
    Calculator::Clear();

    if(p_calculations) {
      p_calculations->clear();
    }

    if(p_methods) {
      p_methods->clear();
    }

    if(p_data) {
      p_data->clear();
    }

    if(p_dataDefinitions) {
      p_dataDefinitions->clear();
    }

    if(p_cubeStats) {
      p_cubeStats->clear();
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
   * @throws Isis::iException::Math
   */
  QVector<double> CubeCalculator::RunCalculations(QVector<Buffer *> &cubeData, int curLine, int curBand) {
    // For now we'll only process a single line in this method for our results. In order
    //    to do more powerful indexing, passing a list of cubes and the output cube will
    //    be necessary.
    int methodIndex = 0;
    int dataIndex = 0;
    for(int currentCalculation = 0; currentCalculation < p_calculations->size();
        currentCalculation++) {
      if((*p_calculations)[currentCalculation] == callNextMethod) {
        void (Calculator::*aMethod)() = (*p_methods)[methodIndex];
        (this->*aMethod)();
        methodIndex ++;
      }
      else {
        DataValue &data = (*p_dataDefinitions)[dataIndex];
        if(data.getType() == DataValue::constant) {
          Push(data.getContant());
        }
        else if(data.getType() == DataValue::band) {
          Push(curBand);
        }
        else if(data.getType() == DataValue::line) {
          Push(curLine);
        }
        else if(data.getType() == DataValue::sample) {
          QVector<double> samples;
          samples.resize(p_outputSamples);

          for(int i = 0; i < p_outputSamples; i++) {
            samples[i] = i + 1;
          }

          Push(samples);
        }
        else if(data.getType() == DataValue::cubeData) {
          Push(*cubeData[data.getCubeIndex()]);
        }
        else {
        }

        dataIndex ++;
      }
    }

    if(StackSize() != 1) {
      string msg = "Too many operands in the equation.";
      throw Isis::iException::Message(Isis::iException::Math, msg, _FILEINFO_);
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
  void CubeCalculator::PrepareCalculations(iString equation,
      QVector<Cube *> &inCubes,
      Cube *outCube) {
    Clear();

    p_outputSamples = outCube->getSampleCount();

    iString eq = equation;
    while(eq != "") {
      iString token = eq.Token(" ");

      // Step through every part of the postfix equation and set up the appropriate
      // action list based on the current token. Attempting to order if-else conditions
      // in terms of what would probably be encountered more often.

      // Scalars
      if(isdigit(token[0]) || token[0] == '.') {
        p_calculations->push_back(pushNextData);
        p_dataDefinitions->push_back(DataValue(DataValue::constant,
                                               token.ToDouble()));
      }
      // File, e.g. F1 = first file in list. Must come after any functions starting with 'f' that
      //   is not a cube.
      else if(token[0] == 'f') {
        iString tok(token.substr(1));
        int file = tok.ToInteger() - 1;
        if(file < 0 || file >= (int)inCubes.size()) {
          std::string msg = "Invalid file number [" + tok + "]";
          throw Isis::iException::Message(Isis::iException::Math, msg, _FILEINFO_);
        }

        p_calculations->push_back(pushNextData);
        p_dataDefinitions->push_back(DataValue(DataValue::cubeData, file));
      }
      else if(token == "band") {
        p_calculations->push_back(pushNextData);
        p_dataDefinitions->push_back(DataValue(DataValue::band));
      }
      else if(token == "line") {
        p_calculations->push_back(pushNextData);
        p_dataDefinitions->push_back(DataValue(DataValue::line));
      }
      else if(token == "sample") {
        p_calculations->push_back(pushNextData);
        p_dataDefinitions->push_back(DataValue(DataValue::sample));
      }
      // Addition
      else if(token == "+") {
        AddMethodCall(&Isis::Calculator::Add);
      }

      // Subtraction
      else if(token == "-") {
        AddMethodCall(&Isis::Calculator::Subtract);
      }

      // Multiplication
      else if(token == "*") {
        AddMethodCall(&Isis::Calculator::Multiply);
      }

      // Division
      else if(token == "/") {
        AddMethodCall(&Isis::Calculator::Divide);
      }

      // Modulus
      else if(token == "%") {
        AddMethodCall(&Isis::Calculator::Modulus);
      }

      // Exponent
      else if(token == "^") {
        AddMethodCall(&Isis::Calculator::Exponent);
      }

      // Negative
      else if(token == "--") {
        AddMethodCall(&Isis::Calculator::Negative);
      }

      // Left shift
      else if(token == "<<") {
        AddMethodCall(&Isis::Calculator::LeftShift);
      }

      // Right shift
      else if(token == ">>") {
        AddMethodCall(&Isis::Calculator::RightShift);
      }

      // Maximum In The Line
      else if(token == "linemax") {
        AddMethodCall(&Isis::Calculator::MaximumLine);
      }

      // Maximum Pixel on a per-pixel basis
      else if(token == "max") {
        AddMethodCall(&Isis::Calculator::MaximumPixel);
      }

      // Minimum In The Line
      else if(token == "linemin") {
        AddMethodCall(&Isis::Calculator::MinimumLine);
      }

      // Minimum Pixel on a per-pixel basis
      else if(token == "min") {
        AddMethodCall(&Isis::Calculator::MinimumPixel);
      }

      // Absolute value
      else if(token == "abs") {
        AddMethodCall(&Isis::Calculator::AbsoluteValue);
      }

      // Square root
      else if(token == "sqrt") {
        AddMethodCall(&Isis::Calculator::SquareRoot);
      }

      // Natural Log
      else if(token == "log" || token == "ln") {
        AddMethodCall(&Isis::Calculator::Log);
      }

      // Log base 10
      else if(token == "log10") {
        AddMethodCall(&Isis::Calculator::Log10);
      }

      // Pi
      else if(token == "pi") {
        p_calculations->push_back(pushNextData);
        p_dataDefinitions->push_back(
          DataValue(DataValue::constant, Isis::PI)
        );
      }

      // e
      else if(token == "e") {
        p_calculations->push_back(pushNextData);
        p_dataDefinitions->push_back(
          DataValue(DataValue::constant, Isis::E)
        );
      }

      // Sine
      else if(token == "sin") {
        AddMethodCall(&Isis::Calculator::Sine);
      }

      // Cosine
      else if(token == "cos") {
        AddMethodCall(&Isis::Calculator::Cosine);
      }

      // Tangent
      else if(token == "tan") {
        AddMethodCall(&Isis::Calculator::Tangent);
      }

      // Secant
      else if(token == "sec") {
        AddMethodCall(&Isis::Calculator::Secant);
      }

      // Cosecant
      else if(token == "csc") {
        AddMethodCall(&Isis::Calculator::Cosecant);
      }

      // Cotangent
      else if(token == "cot") {
        AddMethodCall(&Isis::Calculator::Cotangent);
      }

      // Arcsin
      else if(token == "asin") {
        AddMethodCall(&Isis::Calculator::Arcsine);
      }

      // Arccos
      else if(token == "acos") {
        AddMethodCall(&Isis::Calculator::Arccosine);
      }

      // Arctan
      else if(token == "atan") {
        AddMethodCall(&Isis::Calculator::Arctangent);
      }

      // Arctan2
      else if(token == "atan2") {
        AddMethodCall(&Isis::Calculator::Arctangent2);
      }

      // SineH
      else if(token == "sinh") {
        AddMethodCall(&Isis::Calculator::SineH);
      }

      // CosH
      else if(token == "cosh") {
        AddMethodCall(&Isis::Calculator::CosineH);
      }

      // TanH
      else if(token == "tanh") {
        AddMethodCall(&Isis::Calculator::TangentH);
      }

      // Less than
      else if(token == "<") {
        AddMethodCall(&Isis::Calculator::LessThan);
      }

      // Greater than
      else if(token == ">") {
        AddMethodCall(&Isis::Calculator::GreaterThan);
      }

      // Less than or equal
      else if(token == "<=") {
        AddMethodCall(&Isis::Calculator::LessThanOrEqual);
      }

      // Greater than or equal
      else if(token == ">=") {
        AddMethodCall(&Isis::Calculator::GreaterThanOrEqual);
      }

      // Equal
      else if(token == "==") {
        AddMethodCall(&Isis::Calculator::Equal);
      }

      // Not equal
      else if(token == "!=") {
        AddMethodCall(&Isis::Calculator::NotEqual);
      }

      // Maximum in a cube
      else if(token == "cubemax") {
        int cubeIndex = LastPushToCubeStats(inCubes);

        p_calculations->push_back(pushNextData);
        p_dataDefinitions->push_back(
          DataValue(DataValue::constant, (*p_cubeStats)[cubeIndex]->Maximum())
        );
      }

      // Maximum in a cube
      else if(token == "cubemin") {
        int cubeIndex = LastPushToCubeStats(inCubes);

        p_calculations->push_back(pushNextData);
        p_dataDefinitions->push_back(
          DataValue(DataValue::constant, (*p_cubeStats)[cubeIndex]->Minimum())
        );
      }

      // Ignore empty token
      else if(token == "") { }

      else {
        string msg = "Unidentified operator [";
        msg += token + "]";
        throw Isis::iException::Message(Isis::iException::Math, msg, _FILEINFO_);
      }
    } // while loop
  }

  int CubeCalculator::LastPushToCubeStats(QVector<Cube *> &inCubes) {
    if(!p_calculations->size()) {
      string msg = "Not sure which file to get statistics from";
      throw Isis::iException::Message(Isis::iException::Math, msg, _FILEINFO_);
    }

    if((*p_calculations)[p_calculations->size() - 1] != pushNextData) {
      string msg = "This function must not contain calculations,";
      msg += " only input cubes may be specified.";
      throw Isis::iException::Message(Isis::iException::Math, msg, _FILEINFO_);
    }

    p_calculations->pop_back();

    // This must have data if calculations had data that equaled push data
    DataValue lastData = (*p_dataDefinitions)[p_dataDefinitions->size() - 1];

    if(lastData.getType() != DataValue::cubeData) {
      string msg = "This function must not contain constants,";
      msg += " only input cubes may be specified.";
      throw Isis::iException::Message(Isis::iException::Math, msg, _FILEINFO_);
    }

    int cubeStatsIndex = lastData.getCubeIndex();
    p_dataDefinitions->pop_back();

    // Member variables are now cleaned up, we need to verify the stats exists

    // Make sure room exists in the vector
    while(p_cubeStats->size() < cubeStatsIndex + 1) {
      p_cubeStats->push_back(NULL);
    }

    // Now we can for sure put the stats object in the right place... put it
    //   there
    if((*p_cubeStats)[cubeStatsIndex] == NULL) {
      (*p_cubeStats)[cubeStatsIndex] = inCubes[cubeStatsIndex]->getStatistics();
    }

    return cubeStatsIndex;
  }

  /**
   * This is a conveinience method for PrepareCalculations(...).
   * This will cause RunCalculations(...) to execute this method in order.
   *
   * @param method The method to call, i.e. &Isis::Calculator::Multiply
   */
  void CubeCalculator::AddMethodCall(void (Calculator::*method)(void)) {
    p_calculations->push_back(callNextMethod);
    p_methods->push_back(method);
  }
} // End of namespace Isis
