/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AdjustedRadiusSigmaFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  AdjustedRadiusSigmaFilter::AdjustedRadiusSigmaFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedRadiusSigmaFilter::AdjustedRadiusSigmaFilter(const AdjustedRadiusSigmaFilter &other)
        : AbstractNumberFilter(other) {
  }


  AdjustedRadiusSigmaFilter::~AdjustedRadiusSigmaFilter() {
  }


  bool AdjustedRadiusSigmaFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AdjustedRadiusSigmaFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAdjustedSurfacePoint().GetLocalRadiusSigma().meters());
  }


  bool AdjustedRadiusSigmaFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedRadiusSigmaFilter::clone() const {
    return new AdjustedRadiusSigmaFilter(*this);
  }


  QString AdjustedRadiusSigmaFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point radius sigma "
          "which is ";
    else
      description += "points that have adjusted surface point radius sigmas "
          "which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedRadiusSigmaFilter::getPointDescription() const {
    return "have adjusted surface point radius sigmas which are " +
        descriptionSuffix();
  }
}
