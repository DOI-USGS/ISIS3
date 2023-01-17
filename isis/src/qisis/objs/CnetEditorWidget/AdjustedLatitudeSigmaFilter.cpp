/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AdjustedLatitudeSigmaFilter.h"

#include <QPair>
#include <QString>

#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "Latitude.h"


namespace Isis {
  AdjustedLatitudeSigmaFilter::AdjustedLatitudeSigmaFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedLatitudeSigmaFilter::AdjustedLatitudeSigmaFilter(
        const AdjustedLatitudeSigmaFilter &other) : AbstractNumberFilter(other) {
  }


  AdjustedLatitudeSigmaFilter::~AdjustedLatitudeSigmaFilter() {
  }


  bool AdjustedLatitudeSigmaFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AdjustedLatitudeSigmaFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAdjustedSurfacePoint().GetLatSigmaDistance().meters());
  }


  bool AdjustedLatitudeSigmaFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedLatitudeSigmaFilter::clone() const {
    return new AdjustedLatitudeSigmaFilter(*this);
  }


  QString AdjustedLatitudeSigmaFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point latitude "
          "sigma which is ";
    else
      description += "points that have adjusted surface point latitude "
          "sigmas which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedLatitudeSigmaFilter::getPointDescription() const {
    return "have adjusted surface point latitude sigmas which are " +
        descriptionSuffix();
  }
}
