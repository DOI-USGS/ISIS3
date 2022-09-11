/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AdjustedLongitudeSigmaFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Longitude.h"


namespace Isis {
  AdjustedLongitudeSigmaFilter::AdjustedLongitudeSigmaFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedLongitudeSigmaFilter::AdjustedLongitudeSigmaFilter(
        const AdjustedLongitudeSigmaFilter &other) :
        AbstractNumberFilter(other) {
  }


  AdjustedLongitudeSigmaFilter::~AdjustedLongitudeSigmaFilter() {
  }


  bool AdjustedLongitudeSigmaFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AdjustedLongitudeSigmaFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAdjustedSurfacePoint().GetLonSigmaDistance().meters());
  }


  bool AdjustedLongitudeSigmaFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedLongitudeSigmaFilter::clone() const {
    return new AdjustedLongitudeSigmaFilter(*this);
  }


  QString AdjustedLongitudeSigmaFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point longitude "
          "sigma which is ";
    else
      description += "points that have adjusted surface point longitude "
          "sigmas which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedLongitudeSigmaFilter::getPointDescription() const {
    return "have adjusted surface point longitude sigmas which are " +
        descriptionSuffix();
  }
}
