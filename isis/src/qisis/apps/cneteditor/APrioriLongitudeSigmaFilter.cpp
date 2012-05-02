#include "IsisDebug.h"

#include "APrioriLongitudeSigmaFilter.h"

#include "ControlPoint.h"
#include "Longitude.h"


namespace Isis {
  namespace CnetViz {
    APrioriLongitudeSigmaFilter::APrioriLongitudeSigmaFilter(
      AbstractFilter::FilterEffectivenessFlag flag,
      int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
    }


    APrioriLongitudeSigmaFilter::APrioriLongitudeSigmaFilter(
      const APrioriLongitudeSigmaFilter & other) : AbstractNumberFilter(other) {
    }


    APrioriLongitudeSigmaFilter::~APrioriLongitudeSigmaFilter() {
    }


    bool APrioriLongitudeSigmaFilter::evaluate(
        const ControlCubeGraphNode * node) const {
      return evaluateImageFromPointFilter(node);
    }


    bool APrioriLongitudeSigmaFilter::evaluate(
        const ControlPoint * point) const {
      return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetLonSigmaDistance().meters());
    }


    bool APrioriLongitudeSigmaFilter::evaluate(
        const ControlMeasure * measure) const {
      return true;
    }


    AbstractFilter * APrioriLongitudeSigmaFilter::clone() const {
      return new APrioriLongitudeSigmaFilter(*this);
    }


    QString APrioriLongitudeSigmaFilter::getImageDescription() const {
      QString description = AbstractFilter::getImageDescription();
      if (getMinForSuccess() == 1)
        description += "point that has an <i>a priori</i> surface point "
                       "longitude sigma which is ";
      else
        description += "points that have <i>a priori</i> surface point "
                       "longitude sigmas which are ";

      description += descriptionSuffix();
      return description;
    }


    QString APrioriLongitudeSigmaFilter::getPointDescription() const {
      return "have <i>a priori</i> surface point longitude sigmas which are " +
             descriptionSuffix();
    }
  }
}

