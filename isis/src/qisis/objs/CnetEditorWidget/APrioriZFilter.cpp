/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "APrioriZFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  APrioriZFilter::APrioriZFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  APrioriZFilter::APrioriZFilter(const APrioriZFilter &other)
        : AbstractNumberFilter(other) {
  }


  APrioriZFilter::~APrioriZFilter() {
  }


  bool APrioriZFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool APrioriZFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetZ().meters());
  }


  bool APrioriZFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *APrioriZFilter::clone() const {
    return new APrioriZFilter(*this);
  }


  QString APrioriZFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an <i>a priori</i> surface point "
          "Z which is ";
    else
      description += "points that have <i>a priori</i> surface point "
          "Zs which are ";

    description += descriptionSuffix();
    return description;
  }


  QString APrioriZFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point Zs which are " +
        descriptionSuffix();
  }
}
