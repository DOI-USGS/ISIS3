/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "APrioriLatitudeFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Latitude.h"


namespace Isis {
  APrioriLatitudeFilter::APrioriLatitudeFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  APrioriLatitudeFilter::APrioriLatitudeFilter(const APrioriLatitudeFilter &other)
        : AbstractNumberFilter(other) {
  }


  APrioriLatitudeFilter::~APrioriLatitudeFilter() {
  }


  bool APrioriLatitudeFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool APrioriLatitudeFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetLatitude().degrees());
  }


  bool APrioriLatitudeFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *APrioriLatitudeFilter::clone() const {
    return new APrioriLatitudeFilter(*this);
  }


  QString APrioriLatitudeFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an <i>a priori</i> surface point "
          "latitude which is ";
    else
      description += "points that have <i>a priori</i> surface point "
          "latitudes which are ";

    description += descriptionSuffix();
    return description;
  }


  QString APrioriLatitudeFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point latitudes which are " +
        descriptionSuffix();
  }
}
