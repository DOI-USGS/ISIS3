/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AdjustedZFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  AdjustedZFilter::AdjustedZFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedZFilter::AdjustedZFilter(const AdjustedZFilter &other)
        : AbstractNumberFilter(other) {
  }


  AdjustedZFilter::~AdjustedZFilter() {
  }


  bool AdjustedZFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AdjustedZFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAdjustedSurfacePoint().GetZ().meters());
  }


  bool AdjustedZFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedZFilter::clone() const {
    return new AdjustedZFilter(*this);
  }


  QString AdjustedZFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point Z "
          "which is ";
    else
      description += "points that have adjusted surface point Zs "
          "which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedZFilter::getPointDescription() const {
    return "have adjusted surface point Zs which are " +
        descriptionSuffix();
  }
}
