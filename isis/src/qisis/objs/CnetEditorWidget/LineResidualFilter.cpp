/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineResidualFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  LineResidualFilter::LineResidualFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  LineResidualFilter::LineResidualFilter(const LineResidualFilter &other)
        : AbstractNumberFilter(other) {
  }


  LineResidualFilter::~LineResidualFilter() {
  }


  bool LineResidualFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromMeasureFilter(imageAndNet);
  }


  bool LineResidualFilter::evaluate(const ControlPoint *point) const {
    return evaluatePointFromMeasureFilter(point);
  }


  bool LineResidualFilter::evaluate(const ControlMeasure *measure) const {
    return AbstractNumberFilter::evaluate(measure->GetLineResidual());
  }


  AbstractFilter *LineResidualFilter::clone() const {
    return new LineResidualFilter(*this);
  }


  QString LineResidualFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();

    if (getMinForSuccess() == 1) {
      description += "measure that has a line residual which is ";
    }
    else {
      description += "measures that have line residuals which are ";
    }

    description += descriptionSuffix();
    return description;
  }


  QString LineResidualFilter::getPointDescription() const {
    return getImageDescription();
  }


  QString LineResidualFilter::getMeasureDescription() const {
    return "have line residuals which are " + descriptionSuffix();
  }
}
