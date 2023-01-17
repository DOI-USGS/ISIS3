/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  LineFilter::LineFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  LineFilter::LineFilter(const LineFilter &other) : AbstractNumberFilter(other) {
  }


  LineFilter::~LineFilter() {
  }


  bool LineFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromMeasureFilter(imageAndNet);
  }


  bool LineFilter::evaluate(const ControlPoint *point) const {
    return evaluatePointFromMeasureFilter(point);
  }


  bool LineFilter::evaluate(const ControlMeasure *measure) const {
    return AbstractNumberFilter::evaluate(measure->GetLine());
  }


  AbstractFilter *LineFilter::clone() const {
    return new LineFilter(*this);
  }


  QString LineFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1) {
      description += "measure that has a line which is ";
    }
    else {
      description += "measures that have lines which are ";
    }

    description += descriptionSuffix();
    return description;
  }


  QString LineFilter::getPointDescription() const {
    return getImageDescription();
  }


  QString LineFilter::getMeasureDescription() const {
    return "have lines which are " + descriptionSuffix();
  }
}
