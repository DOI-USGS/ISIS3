/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "MeasureIgnoredFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  MeasureIgnoredFilter::MeasureIgnoredFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractFilter(flag, minimumForSuccess) {
  }


  MeasureIgnoredFilter::~MeasureIgnoredFilter() {
  }


  bool MeasureIgnoredFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return AbstractFilter::evaluateImageFromMeasureFilter(imageAndNet);
  }


  bool MeasureIgnoredFilter::evaluate(const ControlPoint *point) const {
    return AbstractFilter::evaluatePointFromMeasureFilter(point);
  }


  bool MeasureIgnoredFilter::evaluate(const ControlMeasure *measure) const {
    return AbstractFilter::evaluate(measure, &ControlMeasure::IsIgnored);
  }


  AbstractFilter *MeasureIgnoredFilter::clone() const {
    return new MeasureIgnoredFilter(*this);
  }


  QString MeasureIgnoredFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1) {
      description += "measure that is ";
    }
    else {
      description += "measures that are ";
    }

    if (inclusive()) {
      description += "ignored";
    }
    else {
      description += "not ignored";
    }

    return description;
  }


  QString MeasureIgnoredFilter::getPointDescription() const {
    return getImageDescription();
  }


  QString MeasureIgnoredFilter::getMeasureDescription() const {
    QString description = "are ";

    if (inclusive()) {
      description += "ignored";
    }
    else {
      description += "not ignored";
    }

    return description;
  }
}
