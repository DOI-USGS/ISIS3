#include "IsisDebug.h"

#include "APrioriLongitudeFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Longitude.h"


namespace Isis {
  APrioriLongitudeFilter::APrioriLongitudeFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  APrioriLongitudeFilter::APrioriLongitudeFilter(const APrioriLongitudeFilter &other)
        : AbstractNumberFilter(other) {
  }


  APrioriLongitudeFilter::~APrioriLongitudeFilter() {
  }


  bool APrioriLongitudeFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool APrioriLongitudeFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetLongitude().degrees());
  }


  bool APrioriLongitudeFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *APrioriLongitudeFilter::clone() const {
    return new APrioriLongitudeFilter(*this);
  }


  QString APrioriLongitudeFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an <i>a priori</i> surface point "
          "longitude which is ";
    else
      description += "points that have <i>a priori</i> surface point "
          "longitudes which are ";

    description += descriptionSuffix();
    return description;
  }


  QString APrioriLongitudeFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point longitudes which are " +
        descriptionSuffix();
  }
}
