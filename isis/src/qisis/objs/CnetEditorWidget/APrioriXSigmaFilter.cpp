/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "APrioriXSigmaFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  APrioriXSigmaFilter::APrioriXSigmaFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  APrioriXSigmaFilter::APrioriXSigmaFilter(const APrioriXSigmaFilter &other)
        : AbstractNumberFilter(other) {
  }


  APrioriXSigmaFilter::~APrioriXSigmaFilter() {
  }


  bool APrioriXSigmaFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool APrioriXSigmaFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetXSigma().meters());
  }


  bool APrioriXSigmaFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *APrioriXSigmaFilter::clone() const {
    return new APrioriXSigmaFilter(*this);
  }


  QString APrioriXSigmaFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an <i>a priori</i> surface point "
          "X sigma which is ";
    else
      description += "points that have <i>a priori</i> surface point "
          "X sigmas which are ";

    description += descriptionSuffix();
    return description;
  }


  QString APrioriXSigmaFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point X sigmas which are " +
          descriptionSuffix();
  }
}
