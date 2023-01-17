/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "GoodnessOfFitFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlMeasureLogData.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "SpecialPixel.h"


namespace Isis {
  GoodnessOfFitFilter::GoodnessOfFitFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  GoodnessOfFitFilter::GoodnessOfFitFilter(const GoodnessOfFitFilter &other)
        : AbstractNumberFilter(other) {
  }


  GoodnessOfFitFilter::~GoodnessOfFitFilter() {
  }


  bool GoodnessOfFitFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromMeasureFilter(imageAndNet);
  }


  bool GoodnessOfFitFilter::evaluate(const ControlPoint *point) const {
    return evaluatePointFromMeasureFilter(point);
  }


  bool GoodnessOfFitFilter::evaluate(const ControlMeasure *measure) const {
    double goodness = Null;

    if (measure->HasLogData(ControlMeasureLogData::GoodnessOfFit)) {
      goodness = measure->GetLogData(ControlMeasureLogData::GoodnessOfFit).GetNumericalValue();
    }

    return AbstractNumberFilter::evaluate(goodness);
  }


  AbstractFilter *GoodnessOfFitFilter::clone() const {
    return new GoodnessOfFitFilter(*this);
  }


  QString GoodnessOfFitFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "measure that has a goodness of fit which is ";
    else
      description += "measures that have goodness of fits which are ";

    description += descriptionSuffix();
    return description;
  }


  QString GoodnessOfFitFilter::getPointDescription() const {
    return getImageDescription();
  }


  QString GoodnessOfFitFilter::getMeasureDescription() const {
    return "that have goodness of fits which are " + descriptionSuffix();
  }
}
