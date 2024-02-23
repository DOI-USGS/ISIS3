/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "APrioriXFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  APrioriXFilter::APrioriXFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  APrioriXFilter::APrioriXFilter(const APrioriXFilter &other)
        : AbstractNumberFilter(other) {
  }


  APrioriXFilter::~APrioriXFilter() {
  }


  bool APrioriXFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool APrioriXFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetX().kilometers());
  }


  bool APrioriXFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *APrioriXFilter::clone() const {
    return new APrioriXFilter(*this);
  }


  QString APrioriXFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an <i>a priori</i> surface point "
          "X which is ";
    else
      description += "points that have <i>a priori</i> surface point "
          "Xs which are ";

    description += descriptionSuffix();
    return description;
  }


  QString APrioriXFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point Xs which are " +
        descriptionSuffix();
  }
}
