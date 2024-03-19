/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AdjustedXFilter.h"

#include <QPair>
#include <QString>

#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  AdjustedXFilter::AdjustedXFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedXFilter::AdjustedXFilter(
        const AdjustedXFilter &other) : AbstractNumberFilter(other) {
  }


  AdjustedXFilter::~AdjustedXFilter() {
  }


  bool AdjustedXFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AdjustedXFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAdjustedSurfacePoint().GetX().meters());
  }


  bool AdjustedXFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedXFilter::clone() const {
    return new AdjustedXFilter(*this);
  }


  QString AdjustedXFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point X "
          "which is ";
    else
      description += "points that have adjusted surface point Xs "
          "which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedXFilter::getPointDescription() const {
    return "have adjusted surface point Xs which are " +
        descriptionSuffix();
  }
}
