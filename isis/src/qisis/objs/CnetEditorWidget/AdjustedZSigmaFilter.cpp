/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AdjustedZSigmaFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  AdjustedZSigmaFilter::AdjustedZSigmaFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedZSigmaFilter::AdjustedZSigmaFilter(const AdjustedZSigmaFilter &other)
        : AbstractNumberFilter(other) {
  }


  AdjustedZSigmaFilter::~AdjustedZSigmaFilter() {
  }


  bool AdjustedZSigmaFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AdjustedZSigmaFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAdjustedSurfacePoint().GetZSigma().meters());
  }


  bool AdjustedZSigmaFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedZSigmaFilter::clone() const {
    return new AdjustedZSigmaFilter(*this);
  }


  QString AdjustedZSigmaFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point Z sigma "
          "which is ";
    else
      description += "points that have adjusted surface point Z sigmas "
          "which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedZSigmaFilter::getPointDescription() const {
    return "have adjusted surface point Z sigmas which are " +
        descriptionSuffix();
  }
}
