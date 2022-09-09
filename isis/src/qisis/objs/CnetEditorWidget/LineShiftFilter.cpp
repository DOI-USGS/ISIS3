/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineShiftFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  LineShiftFilter::LineShiftFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  LineShiftFilter::LineShiftFilter(const LineShiftFilter &other) : AbstractNumberFilter(other) {
  }


  LineShiftFilter::~LineShiftFilter() {
  }


  bool LineShiftFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromMeasureFilter(imageAndNet);
  }


  bool LineShiftFilter::evaluate(const ControlPoint *point) const {
    return evaluatePointFromMeasureFilter(point);
  }


  bool LineShiftFilter::evaluate(const ControlMeasure *measure) const {
    return AbstractNumberFilter::evaluate(measure->GetLineShift());
  }


  AbstractFilter *LineShiftFilter::clone() const {
    return new LineShiftFilter(*this);
  }


  QString LineShiftFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "measure that has a line shift which is ";
    else
      description += "measures that have line shifts which are ";

    description += descriptionSuffix();
    return description;
  }


  QString LineShiftFilter::getPointDescription() const {
    return getImageDescription();
  }


  QString LineShiftFilter::getMeasureDescription() const {
    return "have line shifts which are " + descriptionSuffix();
  }
}
