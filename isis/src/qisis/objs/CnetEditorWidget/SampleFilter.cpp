/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "SampleFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  SampleFilter::SampleFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  SampleFilter::SampleFilter(const SampleFilter &other) : AbstractNumberFilter(other) {
  }


  SampleFilter::~SampleFilter() {
  }


  bool SampleFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromMeasureFilter(imageAndNet);
  }


  bool SampleFilter::evaluate(const ControlPoint *point) const {
    return evaluatePointFromMeasureFilter(point);
  }


  bool SampleFilter::evaluate(const ControlMeasure *measure) const {
    return AbstractNumberFilter::evaluate(measure->GetSample());
  }


  AbstractFilter *SampleFilter::clone() const {
    return new SampleFilter(*this);
  }


  QString SampleFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1) {
      description += "measure that has a sample which is ";
    }
    else {
      description += "measures that have samples which are ";
    }

    description += descriptionSuffix();
    return description;
  }


  QString SampleFilter::getPointDescription() const {
    return getImageDescription();
  }


  QString SampleFilter::getMeasureDescription() const {
    return "have samples which are " + descriptionSuffix();
  }
}
