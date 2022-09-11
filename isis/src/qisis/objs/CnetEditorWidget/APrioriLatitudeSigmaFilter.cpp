/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "APrioriLatitudeSigmaFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Latitude.h"


namespace Isis {
  APrioriLatitudeSigmaFilter::APrioriLatitudeSigmaFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  APrioriLatitudeSigmaFilter::APrioriLatitudeSigmaFilter(const APrioriLatitudeSigmaFilter &other)
        : AbstractNumberFilter(other) {
  }


  APrioriLatitudeSigmaFilter::~APrioriLatitudeSigmaFilter() {
  }


  bool APrioriLatitudeSigmaFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool APrioriLatitudeSigmaFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetLatSigmaDistance().meters());
  }


  bool APrioriLatitudeSigmaFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *APrioriLatitudeSigmaFilter::clone() const {
    return new APrioriLatitudeSigmaFilter(*this);
  }


  QString APrioriLatitudeSigmaFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an <i>a priori</i> surface point "
          "latitude sigma which is ";
    else
      description += "points that have <i>a priori</i> surface point "
          "latitude sigmas which are ";

    description += descriptionSuffix();
    return description;
  }


  QString APrioriLatitudeSigmaFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point latitude sigmas which are " +
          descriptionSuffix();
  }
}
