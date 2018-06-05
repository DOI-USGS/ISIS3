#include "IsisDebug.h"

#include "AdjustedRadiusFilter.h"

#include "ControlPoint.h"
#include "Longitude.h"


namespace Isis {
  AdjustedRadiusFilter::AdjustedRadiusFilter(
    AbstractFilter::FilterEffectivenessFlag flag,
    ControlNet *network,
    int minimumForSuccess) : AbstractNumberFilter(flag, network, minimumForSuccess) {
  }


  AdjustedRadiusFilter::AdjustedRadiusFilter(
    const AdjustedRadiusFilter &other) : AbstractNumberFilter(other) {
  }


  AdjustedRadiusFilter::~AdjustedRadiusFilter() {
  }


  bool AdjustedRadiusFilter::evaluate(const QString *imageSerial) const {
    return evaluateImageFromPointFilter(imageSerial);
  }


  bool AdjustedRadiusFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
        point->GetAdjustedSurfacePoint().GetLocalRadius().meters());
  }


  bool AdjustedRadiusFilter::evaluate(
    const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedRadiusFilter::clone() const {
    return new AdjustedRadiusFilter(*this);
  }


  QString AdjustedRadiusFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point radius "
          "which is ";
    else
      description += "points that have adjusted surface point radii "
          "which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedRadiusFilter::getPointDescription() const {
    return "have adjusted surface point radii which are " +
        descriptionSuffix();
  }
}
