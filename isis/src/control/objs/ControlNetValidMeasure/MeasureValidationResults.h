#ifndef _MeasureValidationResults_h_
#define _MeasureValidationResults_h_

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

class QString;
template< class T > class QVector;

namespace Isis {
  /**
   * @brief MeasureValidationResults class
   *
   * Stores the error/results string of a Measure's Validation
   *
   * @see cnetref cnetedit etc.
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *  @history 2011-05-11 Sharmila Prasad - Added Residuals for Validation and added
   *                                        comparison string for API addFailure(..)
   */
  class MeasureValidationResults {

    public:
      enum Option {
        EmissionAngle,
        IncidenceAngle,
        DNValue,
        Resolution,
        PixelsFromEdge,
        MetersFromEdge,
        SampleResidual,
        LineResidual,
        ResidualMagnitude,
        SampleShift,
        LineShift,
        PixelShift
      };

      MeasureValidationResults();
      ~MeasureValidationResults();

      bool isValid();
      bool getValidStatus(Option opt);

      QString toString();
      QString toString(QString serialNumber, QString pointID);
      QString toString(QString sample, QString line, QString serialNumber,
          QString pointID);

      void addFailure(Option opt, double tolerance, const char* compare="less");
      void addFailure(Option opt, double computed, double min, double max);

      QString getFailurePrefix(Option opt);

    private:
      QVector<Option> * failures;
      QString * errorMsg;
  };
};

#endif
