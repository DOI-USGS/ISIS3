/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "MeasureValidationResults.h"

#include <QString>
#include <QVector>

namespace Isis {

  MeasureValidationResults::MeasureValidationResults() {
    failures = NULL;
    errorMsg = NULL;

    failures = new QVector<Option>();
    errorMsg = new QString();
  }


  MeasureValidationResults::~MeasureValidationResults() {
    if (failures != NULL) {
      delete failures;
      failures = NULL;
    }
    if (errorMsg != NULL) {
      delete errorMsg;
      errorMsg = NULL;
    }
  }


  bool MeasureValidationResults::isValid() {
    return failures->size() == 0;
  }


  bool MeasureValidationResults::getValidStatus(Option opt) {
    return !failures->contains(opt);
  }


  QString MeasureValidationResults::toString() {
    return ((errorMsg->isEmpty()) ? "succeeded" : (*errorMsg));
  }


  QString MeasureValidationResults::toString(QString serialNumber, QString pointID) {
    return "Control Measure with Serial Number " + serialNumber +
      " in Control Point " + pointID + " " +
      ((errorMsg->isEmpty()) ? "succeeded" : (*errorMsg));
  }


  QString MeasureValidationResults::toString(QString sample, QString line,
      QString serialNumber, QString pointID) {
    return "Control Measure with position (" + sample + ", " + line +
      ") of Serial Number " + serialNumber +
      " in Control Point " + pointID + " " +
      ((errorMsg->isEmpty()) ? "succeeded" : (*errorMsg));
  }


  void MeasureValidationResults::addFailure(Option opt, double tolerance, const char* compare) {
    failures->push_back(opt);

    QString failureReason = getFailurePrefix(opt) + "is " + compare + " than tolerance " +
      QString::number(tolerance);
    errorMsg->append(failureReason);
  }


  void MeasureValidationResults::addFailure(Option opt, double computed, double min, double max) {
    failures->push_back(opt);

    QString failureReason = getFailurePrefix(opt) + QString::number(computed) +
      " is outside range [" + QString::number(min) + ", " +
      QString::number(max) + "]";
    errorMsg->append(failureReason);
  }


  QString MeasureValidationResults::getFailurePrefix(Option opt) {
    QString optString = "\n  ";
    switch (opt) {
      case EmissionAngle:
        optString += "Emission Angle";
        break;
      case IncidenceAngle:
        optString += "Incidence Angle";
        break;
      case DNValue:
        optString += "DN Value";
        break;
      case Resolution:
        optString += "Resolution";
        break;
      case PixelsFromEdge:
        optString += "Pixels From Edge";
        break;
      case MetersFromEdge:
        optString += "Meters From Edge";
        break;
      case SampleResidual:
        optString += "Sample Residual";
        break;
      case LineResidual:
        optString += "Line Residual";
        break;
      case ResidualMagnitude:
        optString += "Residual Magnitude";
        break;
      case SampleShift:
        optString += "Sample Shift";
        break;
      case LineShift:
        optString += "Line Shift";
        break;
      case PixelShift:
        optString += "Pixel Shift";
        break;
    }
    optString += " ";

    return ((errorMsg->isEmpty()) ? "failed: " : "") + optString;
  }
};
