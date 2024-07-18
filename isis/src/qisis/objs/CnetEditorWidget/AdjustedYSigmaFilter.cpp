/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AdjustedYSigmaFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  AdjustedYSigmaFilter::AdjustedYSigmaFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedYSigmaFilter::AdjustedYSigmaFilter(
        const AdjustedYSigmaFilter &other) :
        AbstractNumberFilter(other) {
  }


  AdjustedYSigmaFilter::~AdjustedYSigmaFilter() {
  }


  bool AdjustedYSigmaFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AdjustedYSigmaFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAdjustedSurfacePoint().GetYSigma().meters());
  }


  bool AdjustedYSigmaFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedYSigmaFilter::clone() const {
    return new AdjustedYSigmaFilter(*this);
  }


  QString AdjustedYSigmaFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point Y "
          "sigma which is ";
    else
      description += "points that have adjusted surface point Y "
          "sigmas which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedYSigmaFilter::getPointDescription() const {
    return "have adjusted surface point Y sigmas which are " +
        descriptionSuffix();
  }
}
