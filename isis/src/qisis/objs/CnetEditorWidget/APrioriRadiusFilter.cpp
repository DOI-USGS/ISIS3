#include "IsisDebug.h"

#include "APrioriRadiusFilter.h"

#include "ControlPoint.h"
#include "Longitude.h"


namespace Isis {
  namespace CnetViz {
    APrioriRadiusFilter::APrioriRadiusFilter(
      AbstractFilter::FilterEffectivenessFlag flag,
      int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
    }


    APrioriRadiusFilter::APrioriRadiusFilter(
      const APrioriRadiusFilter &other) : AbstractNumberFilter(other) {
    }


    APrioriRadiusFilter::~APrioriRadiusFilter() {
    }


    bool APrioriRadiusFilter::evaluate(
      const ControlCubeGraphNode *node) const {
      return evaluateImageFromPointFilter(node);
    }


    bool APrioriRadiusFilter::evaluate(const ControlPoint *point) const {
      return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetLocalRadius().meters());
    }


    bool APrioriRadiusFilter::evaluate(
      const ControlMeasure *measure) const {
      return true;
    }


    AbstractFilter *APrioriRadiusFilter::clone() const {
      return new APrioriRadiusFilter(*this);
    }


    QString APrioriRadiusFilter::getImageDescription() const {
      QString description = AbstractFilter::getImageDescription();
      if (getMinForSuccess() == 1)
        description += "point that has an <i>a priori</i> surface point "
            "radius which is ";
      else
        description += "points that have <i>a priori</i> surface point "
            "radii which are ";

      description += descriptionSuffix();
      return description;
    }


    QString APrioriRadiusFilter::getPointDescription() const {
      return "have <i>a priori</i> surface point radii which are " +
          descriptionSuffix();
    }
  }
}

