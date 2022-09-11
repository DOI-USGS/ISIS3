/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "APrioriRadiusSigmaFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  APrioriRadiusSigmaFilter::APrioriRadiusSigmaFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  APrioriRadiusSigmaFilter::APrioriRadiusSigmaFilter(const APrioriRadiusSigmaFilter &other)
        : AbstractNumberFilter(other) {
  }


  APrioriRadiusSigmaFilter::~APrioriRadiusSigmaFilter() {
  }


  bool APrioriRadiusSigmaFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool APrioriRadiusSigmaFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetLocalRadiusSigma().meters());
  }


  bool APrioriRadiusSigmaFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *APrioriRadiusSigmaFilter::clone() const {
    return new APrioriRadiusSigmaFilter(*this);
  }


  QString APrioriRadiusSigmaFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an <i>a priori</i> surface point "
          "radius sigma which is ";
    else
      description += "points that have <i>a priori</i> surface point "
          "radius sigmas which are ";

    description += descriptionSuffix();
    return description;
  }


  QString APrioriRadiusSigmaFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point radius sigmas which are " +
        descriptionSuffix();
  }
}
