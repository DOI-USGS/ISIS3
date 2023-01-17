/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "APrioriLongitudeSigmaFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Longitude.h"


namespace Isis {
  APrioriLongitudeSigmaFilter::APrioriLongitudeSigmaFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  APrioriLongitudeSigmaFilter::APrioriLongitudeSigmaFilter(
        const APrioriLongitudeSigmaFilter &other) : AbstractNumberFilter(other) {
  }


  APrioriLongitudeSigmaFilter::~APrioriLongitudeSigmaFilter() {
  }


  bool APrioriLongitudeSigmaFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool APrioriLongitudeSigmaFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetLonSigmaDistance().meters());
  }


  bool APrioriLongitudeSigmaFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *APrioriLongitudeSigmaFilter::clone() const {
    return new APrioriLongitudeSigmaFilter(*this);
  }


  QString APrioriLongitudeSigmaFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an <i>a priori</i> surface point "
          "longitude sigma which is ";
    else
      description += "points that have <i>a priori</i> surface point "
          "longitude sigmas which are ";

    description += descriptionSuffix();
    return description;
  }


  QString APrioriLongitudeSigmaFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point longitude sigmas which are " +
        descriptionSuffix();
  }
}
