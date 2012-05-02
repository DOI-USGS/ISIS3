#include "IsisDebug.h"

#include "AdjustedRadiusFilter.h"

#include "ControlPoint.h"
#include "Longitude.h"


namespace Isis {
  namespace CnetViz {
    AdjustedRadiusFilter::AdjustedRadiusFilter(
      AbstractFilter::FilterEffectivenessFlag flag,
      int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
    }


    AdjustedRadiusFilter::AdjustedRadiusFilter(
      const AdjustedRadiusFilter & other) : AbstractNumberFilter(other) {
    }


    AdjustedRadiusFilter::~AdjustedRadiusFilter() {
    }


    bool AdjustedRadiusFilter::evaluate(
        const ControlCubeGraphNode * node) const {
      return evaluateImageFromPointFilter(node);
    }


    bool AdjustedRadiusFilter::evaluate(const ControlPoint * point) const {
      return AbstractNumberFilter::evaluate(
          point->GetAdjustedSurfacePoint().GetLocalRadius().meters());
    }


    bool AdjustedRadiusFilter::evaluate(
        const ControlMeasure * measure) const {
      return true;
    }


    AbstractFilter * AdjustedRadiusFilter::clone() const {
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
}

