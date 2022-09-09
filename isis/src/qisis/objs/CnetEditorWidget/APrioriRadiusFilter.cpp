/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "APrioriRadiusFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  APrioriRadiusFilter::APrioriRadiusFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  APrioriRadiusFilter::APrioriRadiusFilter(const APrioriRadiusFilter &other)
        : AbstractNumberFilter(other) {
  }


  APrioriRadiusFilter::~APrioriRadiusFilter() {
  }


  bool APrioriRadiusFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool APrioriRadiusFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetLocalRadius().meters());
  }


  bool APrioriRadiusFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *APrioriRadiusFilter::clone() const {
    return new APrioriRadiusFilter(*this);
  }


  QString APrioriRadiusFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an <i>a priori</i> surface point "
          "radius which is ";
    else
      description += "points that have <i>a priori</i> surface point "
          "radii which are ";

    description += descriptionSuffix();
    return description;
  }


  QString APrioriRadiusFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point radii which are " +
        descriptionSuffix();
  }
}
