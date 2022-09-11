/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "PointIdFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "IString.h"


namespace Isis {
  PointIdFilter::PointIdFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractStringFilter(flag, minimumForSuccess) {
  }


  PointIdFilter::PointIdFilter(const PointIdFilter &other) : AbstractStringFilter(other) {
  }


  PointIdFilter::~PointIdFilter() {
  }


  bool PointIdFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool PointIdFilter::evaluate(const ControlPoint *point) const {
    return AbstractStringFilter::evaluate((QString) point->GetId());
  }


  bool PointIdFilter::evaluate(const ControlMeasure *) const {
    return true;
  }


  AbstractFilter *PointIdFilter::clone() const {
    return new PointIdFilter(*this);
  }


  QString PointIdFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();

    if (getMinForSuccess() == 1) {
      description += "point with it's ID ";
    }
    else {
      description += "points with IDs ";
    }

    description += descriptionSuffix();
    return description;
  }


  QString PointIdFilter::getPointDescription() const {
    return "have IDs " + descriptionSuffix();
  }
}
