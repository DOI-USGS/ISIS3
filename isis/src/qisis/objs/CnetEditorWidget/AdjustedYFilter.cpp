/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AdjustedYFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  AdjustedYFilter::AdjustedYFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess)
        : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedYFilter::AdjustedYFilter(const AdjustedYFilter &other)
        : AbstractNumberFilter(other) {
  }


  AdjustedYFilter::~AdjustedYFilter() {
  }


  bool AdjustedYFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AdjustedYFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAdjustedSurfacePoint().GetY().meters());
  }


  bool AdjustedYFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedYFilter::clone() const {
    return new AdjustedYFilter(*this);
  }


  QString AdjustedYFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point Y "
          "which is ";
    else
      description += "points that have adjusted surface point Ys "
          "which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedYFilter::getPointDescription() const {
    return "have adjusted surface point Ys which are " +
        descriptionSuffix();
  }
}
