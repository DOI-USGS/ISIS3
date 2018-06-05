#include "IsisDebug.h"

#include "AdjustedLongitudeSigmaFilter.h"

#include "ControlPoint.h"
#include "Longitude.h"


namespace Isis {
  AdjustedLongitudeSigmaFilter::AdjustedLongitudeSigmaFilter(
    AbstractFilter::FilterEffectivenessFlag flag,
    ControlNet *network,
    int minimumForSuccess) : AbstractNumberFilter(flag, network, minimumForSuccess) {
  }


  AdjustedLongitudeSigmaFilter::AdjustedLongitudeSigmaFilter(
    const AdjustedLongitudeSigmaFilter &other) :
    AbstractNumberFilter(other) {
  }


  AdjustedLongitudeSigmaFilter::~AdjustedLongitudeSigmaFilter() {
  }


  bool AdjustedLongitudeSigmaFilter::evaluate(const QString *imageSerial) const {
    return evaluateImageFromPointFilter(imageSerial);
  }


  bool AdjustedLongitudeSigmaFilter::evaluate(
    const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
        point->GetAdjustedSurfacePoint().GetLonSigmaDistance().meters());
  }


  bool AdjustedLongitudeSigmaFilter::evaluate(
    const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedLongitudeSigmaFilter::clone() const {
    return new AdjustedLongitudeSigmaFilter(*this);
  }


  QString AdjustedLongitudeSigmaFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point longitude "
          "sigma which is ";
    else
      description += "points that have adjusted surface point longitude "
          "sigmas which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedLongitudeSigmaFilter::getPointDescription() const {
    return "have adjusted surface point longitude sigmas which are " +
        descriptionSuffix();
  }
}
