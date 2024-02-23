/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AdjustedXSigmaFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  AdjustedXSigmaFilter::AdjustedXSigmaFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedXSigmaFilter::AdjustedXSigmaFilter(
        const AdjustedXSigmaFilter &other) : AbstractNumberFilter(other) {
  }


  AdjustedXSigmaFilter::~AdjustedXSigmaFilter() {
  }


  bool AdjustedXSigmaFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AdjustedXSigmaFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAdjustedSurfacePoint().GetXSigma().meters());
  }


  bool AdjustedXSigmaFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedXSigmaFilter::clone() const {
    return new AdjustedXSigmaFilter(*this);
  }


  QString AdjustedXSigmaFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point X "
          "sigma which is ";
    else
      description += "points that have adjusted surface point X "
          "sigmas which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedXSigmaFilter::getPointDescription() const {
    return "have adjusted surface point X sigmas which are " +
        descriptionSuffix();
  }
}
