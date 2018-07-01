#include "IsisDebug.h"

#include "AdjustedLatitudeFilter.h"

#include "ControlPoint.h"
#include "Latitude.h"


namespace Isis {
  AdjustedLatitudeFilter::AdjustedLatitudeFilter(
    AbstractFilter::FilterEffectivenessFlag flag,
    int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedLatitudeFilter::AdjustedLatitudeFilter(
    const AdjustedLatitudeFilter &other) : AbstractNumberFilter(other) {
  }


  AdjustedLatitudeFilter::~AdjustedLatitudeFilter() {
  }


  bool AdjustedLatitudeFilter::evaluate(
    const ControlCubeGraphNode *node) const {
    return evaluateImageFromPointFilter(node);
  }


  bool AdjustedLatitudeFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
        point->GetAdjustedSurfacePoint().GetLatitude().degrees());
  }


  bool AdjustedLatitudeFilter::evaluate(
    const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedLatitudeFilter::clone() const {
    return new AdjustedLatitudeFilter(*this);
  }


  QString AdjustedLatitudeFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point latitude "
          "which is ";
    else
      description += "points that have adjusted surface point latitudes "
          "which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedLatitudeFilter::getPointDescription() const {
    return "have adjusted surface point latitudes which are " +
        descriptionSuffix();
  }
}

