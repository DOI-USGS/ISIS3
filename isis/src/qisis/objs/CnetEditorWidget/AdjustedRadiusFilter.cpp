/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AdjustedRadiusFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  AdjustedRadiusFilter::AdjustedRadiusFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedRadiusFilter::AdjustedRadiusFilter(const AdjustedRadiusFilter &other)
        : AbstractNumberFilter(other) {
  }


  AdjustedRadiusFilter::~AdjustedRadiusFilter() {
  }


  bool AdjustedRadiusFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AdjustedRadiusFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAdjustedSurfacePoint().GetLocalRadius().meters());
  }


  bool AdjustedRadiusFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedRadiusFilter::clone() const {
    return new AdjustedRadiusFilter(*this);
  }


  QString AdjustedRadiusFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point radius "
          "which is ";
    else
      description += "points that have adjusted surface point radii "
          "which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedRadiusFilter::getPointDescription() const {
    return "have adjusted surface point radii which are " +
        descriptionSuffix();
  }
}
