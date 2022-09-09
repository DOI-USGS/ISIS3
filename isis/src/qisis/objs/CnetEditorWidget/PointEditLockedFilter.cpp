/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "PointEditLockedFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  PointEditLockedFilter::PointEditLockedFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractFilter(flag, minimumForSuccess) {
  }


  PointEditLockedFilter::PointEditLockedFilter(const AbstractFilter &other)
        : AbstractFilter(other) {
  }


  PointEditLockedFilter::~PointEditLockedFilter() {
  }


  bool PointEditLockedFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return AbstractFilter::evaluateImageFromPointFilter(imageAndNet);
  }


  bool PointEditLockedFilter::evaluate(const ControlPoint *point) const {
    return AbstractFilter::evaluate(point, &ControlPoint::IsEditLocked);
  }


  bool PointEditLockedFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *PointEditLockedFilter::clone() const {
    return new PointEditLockedFilter(*this);
  }


  QString PointEditLockedFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();

    if (getMinForSuccess() == 1) {
      description += "point that is ";
    }
    else {
      description += "points that are ";
    }

    if (inclusive()) {
      description += "edit locked";
    }
    else {
      description += "not edit locked";
    }

    return description;
  }


  QString PointEditLockedFilter::getPointDescription() const {
    QString description = "are ";

    if (inclusive()) {
      description += "edit locked";
    }
    else {
      description += "not edit locked";
    }

    return description;
  }
}
