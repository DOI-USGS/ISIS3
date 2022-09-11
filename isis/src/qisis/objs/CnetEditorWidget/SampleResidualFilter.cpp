/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "SampleResidualFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  SampleResidualFilter::SampleResidualFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  SampleResidualFilter::SampleResidualFilter(const SampleResidualFilter &other)
        : AbstractNumberFilter(other) {
  }


  SampleResidualFilter::~SampleResidualFilter() {
  }


  bool SampleResidualFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromMeasureFilter(imageAndNet);
  }


  bool SampleResidualFilter::evaluate(const ControlPoint *point) const {
    return evaluatePointFromMeasureFilter(point);
  }


  bool SampleResidualFilter::evaluate(const ControlMeasure *measure) const {
    return AbstractNumberFilter::evaluate(measure->GetSampleResidual());
  }


  AbstractFilter *SampleResidualFilter::clone() const {
    return new SampleResidualFilter(*this);
  }


  QString SampleResidualFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1) {
      description += "measure that has a sample residual which is ";
    }
    else {
      description += "measures that have sample residuals which are ";
    }

    description += descriptionSuffix();
    return description;
  }


  QString SampleResidualFilter::getPointDescription() const {
    return getImageDescription();
  }


  QString SampleResidualFilter::getMeasureDescription() const {
    return "have sample residuals which are " + descriptionSuffix();
  }
}
