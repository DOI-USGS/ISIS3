/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "APrioriZSigmaFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  APrioriZSigmaFilter::APrioriZSigmaFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  APrioriZSigmaFilter::APrioriZSigmaFilter(const APrioriZSigmaFilter &other)
        : AbstractNumberFilter(other) {
  }


  APrioriZSigmaFilter::~APrioriZSigmaFilter() {
  }


  bool APrioriZSigmaFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool APrioriZSigmaFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetZSigma().meters());
  }


  bool APrioriZSigmaFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *APrioriZSigmaFilter::clone() const {
    return new APrioriZSigmaFilter(*this);
  }


  QString APrioriZSigmaFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an <i>a priori</i> surface point "
          "Z sigma which is ";
    else
      description += "points that have <i>a priori</i> surface point "
          "Z sigmas which are ";

    description += descriptionSuffix();
    return description;
  }


  QString APrioriZSigmaFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point Z sigmas which are " +
        descriptionSuffix();
  }
}
