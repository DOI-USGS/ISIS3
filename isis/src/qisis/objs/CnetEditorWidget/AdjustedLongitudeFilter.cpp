#include "IsisDebug.h"

#include "AdjustedLongitudeFilter.h"

#include <QPair>
#include <QString>

#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "Longitude.h"


namespace Isis {
  AdjustedLongitudeFilter::AdjustedLongitudeFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess)
        : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedLongitudeFilter::AdjustedLongitudeFilter(const AdjustedLongitudeFilter &other)
        : AbstractNumberFilter(other) {
  }


  AdjustedLongitudeFilter::~AdjustedLongitudeFilter() {
  }


  bool AdjustedLongitudeFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AdjustedLongitudeFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAdjustedSurfacePoint().GetLongitude().degrees());
  }


  bool AdjustedLongitudeFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedLongitudeFilter::clone() const {
    return new AdjustedLongitudeFilter(*this);
  }


  QString AdjustedLongitudeFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point longitude "
          "which is ";
    else
      description += "points that have adjusted surface point longitudes "
          "which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedLongitudeFilter::getPointDescription() const {
    return "have adjusted surface point longitudes which are " +
        descriptionSuffix();
  }
}
