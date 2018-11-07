#include "IsisDebug.h"

#include "AdjustedRadiusSigmaFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  AdjustedRadiusSigmaFilter::AdjustedRadiusSigmaFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedRadiusSigmaFilter::AdjustedRadiusSigmaFilter(const AdjustedRadiusSigmaFilter &other)
        : AbstractNumberFilter(other) {
  }


  AdjustedRadiusSigmaFilter::~AdjustedRadiusSigmaFilter() {
  }


  bool AdjustedRadiusSigmaFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AdjustedRadiusSigmaFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAdjustedSurfacePoint().GetLocalRadiusSigma().meters());
  }


  bool AdjustedRadiusSigmaFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedRadiusSigmaFilter::clone() const {
    return new AdjustedRadiusSigmaFilter(*this);
  }


  QString AdjustedRadiusSigmaFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point radius sigma "
          "which is ";
    else
      description += "points that have adjusted surface point radius sigmas "
          "which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedRadiusSigmaFilter::getPointDescription() const {
    return "have adjusted surface point radius sigmas which are " +
        descriptionSuffix();
  }
}
