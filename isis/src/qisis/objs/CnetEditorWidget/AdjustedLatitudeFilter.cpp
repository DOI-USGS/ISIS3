/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AdjustedLatitudeFilter.h"

#include <QPair>
#include <QString>

#include "ControlPoint.h"
#include "ControlNet.h"
#include "Latitude.h"


namespace Isis {
  AdjustedLatitudeFilter::AdjustedLatitudeFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedLatitudeFilter::AdjustedLatitudeFilter(
        const AdjustedLatitudeFilter &other) : AbstractNumberFilter(other) {
  }


  AdjustedLatitudeFilter::~AdjustedLatitudeFilter() {
  }


  bool AdjustedLatitudeFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AdjustedLatitudeFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAdjustedSurfacePoint().GetLatitude().degrees());
  }


  bool AdjustedLatitudeFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedLatitudeFilter::clone() const {
    return new AdjustedLatitudeFilter(*this);
  }


  QString AdjustedLatitudeFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point latitude "
          "which is ";
    else
      description += "points that have adjusted surface point latitudes "
          "which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedLatitudeFilter::getPointDescription() const {
    return "have adjusted surface point latitudes which are " +
        descriptionSuffix();
  }
}
