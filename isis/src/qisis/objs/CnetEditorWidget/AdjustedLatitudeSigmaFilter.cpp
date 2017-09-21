#include "IsisDebug.h"

#include "AdjustedLatitudeSigmaFilter.h"

#include "ControlPoint.h"
#include "Latitude.h"


namespace Isis {
  AdjustedLatitudeSigmaFilter::AdjustedLatitudeSigmaFilter(
    AbstractFilter::FilterEffectivenessFlag flag,
    int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedLatitudeSigmaFilter::AdjustedLatitudeSigmaFilter(
    const AdjustedLatitudeSigmaFilter &other) : AbstractNumberFilter(other) {
  }


  AdjustedLatitudeSigmaFilter::~AdjustedLatitudeSigmaFilter() {
  }


  bool AdjustedLatitudeSigmaFilter::evaluate(
    const ControlCubeGraphNode *node) const {
    return evaluateImageFromPointFilter(node);
  }


  bool AdjustedLatitudeSigmaFilter::evaluate(
    const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
        point->GetAdjustedSurfacePoint().GetLatSigmaDistance().meters());
  }


  bool AdjustedLatitudeSigmaFilter::evaluate(
    const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedLatitudeSigmaFilter::clone() const {
    return new AdjustedLatitudeSigmaFilter(*this);
  }


  QString AdjustedLatitudeSigmaFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point latitude "
          "sigma which is ";
    else
      description += "points that have adjusted surface point latitude "
          "sigmas which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedLatitudeSigmaFilter::getPointDescription() const {
    return "have adjusted surface point latitude sigmas which are " +
        descriptionSuffix();
  }
}

