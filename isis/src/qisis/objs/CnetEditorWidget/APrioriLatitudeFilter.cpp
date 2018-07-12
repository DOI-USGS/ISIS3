#include "IsisDebug.h"

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
