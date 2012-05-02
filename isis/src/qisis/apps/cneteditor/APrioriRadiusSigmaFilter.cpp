#include "IsisDebug.h"

#include "APrioriRadiusSigmaFilter.h"

#include "ControlPoint.h"
#include "Longitude.h"


namespace Isis {
  namespace CnetViz {
    APrioriRadiusSigmaFilter::APrioriRadiusSigmaFilter(
      AbstractFilter::FilterEffectivenessFlag flag,
      int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
    }


    APrioriRadiusSigmaFilter::APrioriRadiusSigmaFilter(
      const APrioriRadiusSigmaFilter & other) : AbstractNumberFilter(other) {
    }


    APrioriRadiusSigmaFilter::~APrioriRadiusSigmaFilter() {
    }


    bool APrioriRadiusSigmaFilter::evaluate(
        const ControlCubeGraphNode * node) const {
      return evaluateImageFromPointFilter(node);
    }


    bool APrioriRadiusSigmaFilter::evaluate(const ControlPoint * point) const {
      return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetLocalRadiusSigma().meters());
    }


    bool APrioriRadiusSigmaFilter::evaluate(
        const ControlMeasure * measure) const {
      return true;
    }


    AbstractFilter * APrioriRadiusSigmaFilter::clone() const {
      return new APrioriRadiusSigmaFilter(*this);
    }


    QString APrioriRadiusSigmaFilter::getImageDescription() const {
      QString description = AbstractFilter::getImageDescription();
      if (getMinForSuccess() == 1)
        description += "point that has an <i>a priori</i> surface point "
                       "radius sigma which is ";
      else
        description += "points that have <i>a priori</i> surface point "
                       "radius sigmas which are ";

      description += descriptionSuffix();
      return description;
    }


    QString APrioriRadiusSigmaFilter::getPointDescription() const {
      return "have <i>a priori</i> surface point radius sigmas which are " +
             descriptionSuffix();
    }
  }
}

