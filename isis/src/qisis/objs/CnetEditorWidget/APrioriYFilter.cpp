/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "APrioriYFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  APrioriYFilter::APrioriYFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  APrioriYFilter::APrioriYFilter(const APrioriYFilter &other)
        : AbstractNumberFilter(other) {
  }


  APrioriYFilter::~APrioriYFilter() {
  }


  bool APrioriYFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool APrioriYFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetY().meters());
  }


  bool APrioriYFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *APrioriYFilter::clone() const {
    return new APrioriYFilter(*this);
  }


  QString APrioriYFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an <i>a priori</i> surface point "
          "Y which is ";
    else
      description += "points that have <i>a priori</i> surface point "
          "Ys which are ";

    description += descriptionSuffix();
    return description;
  }


  QString APrioriYFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point Ys which are " +
        descriptionSuffix();
  }
}
