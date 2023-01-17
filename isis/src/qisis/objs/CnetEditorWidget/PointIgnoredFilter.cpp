/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "PointIgnoredFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  PointIgnoredFilter::PointIgnoredFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractFilter(flag, minimumForSuccess) {
  }


  PointIgnoredFilter::PointIgnoredFilter(const AbstractFilter &other) : AbstractFilter(other) {
  }


  PointIgnoredFilter::~PointIgnoredFilter() {
  }


  bool PointIgnoredFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return AbstractFilter::evaluateImageFromPointFilter(imageAndNet);
  }


  bool PointIgnoredFilter::evaluate(const ControlPoint *point) const {
    return AbstractFilter::evaluate(point, &ControlPoint::IsIgnored);
  }


  bool PointIgnoredFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *PointIgnoredFilter::clone() const {
    return new PointIgnoredFilter(*this);
  }


  QString PointIgnoredFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();

    if (getMinForSuccess() == 1) {
      description += "point that is ";
    }
    else {
      description += "points that are ";
    }

    if (inclusive()) {
      description += "ignored";
    }
    else {
      description += "not ignored";
    }

    return description;
  }


  QString PointIgnoredFilter::getPointDescription() const {
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
