#include "IsisDebug.h"

#include "APrioriLatitudeSigmaFilter.h"

#include "ControlPoint.h"
#include "Latitude.h"


namespace Isis {
  namespace CnetViz {
    APrioriLatitudeSigmaFilter::APrioriLatitudeSigmaFilter(
      AbstractFilter::FilterEffectivenessFlag flag,
      int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
    }


    APrioriLatitudeSigmaFilter::APrioriLatitudeSigmaFilter(
      const APrioriLatitudeSigmaFilter &other) : AbstractNumberFilter(other) {
    }


    APrioriLatitudeSigmaFilter::~APrioriLatitudeSigmaFilter() {
    }


    bool APrioriLatitudeSigmaFilter::evaluate(
      const ControlCubeGraphNode *node) const {
      return evaluateImageFromPointFilter(node);
    }


    bool APrioriLatitudeSigmaFilter::evaluate(const ControlPoint *point) const {
      return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetLatSigmaDistance().meters());
    }


    bool APrioriLatitudeSigmaFilter::evaluate(
      const ControlMeasure *measure) const {
      return true;
    }


    AbstractFilter *APrioriLatitudeSigmaFilter::clone() const {
      return new APrioriLatitudeSigmaFilter(*this);
    }


    QString APrioriLatitudeSigmaFilter::getImageDescription() const {
      QString description = AbstractFilter::getImageDescription();
      if (getMinForSuccess() == 1)
        description += "point that has an <i>a priori</i> surface point "
            "latitude sigma which is ";
      else
        description += "points that have <i>a priori</i> surface point "
            "latitude sigmas which are ";

      description += descriptionSuffix();
      return description;
    }


    QString APrioriLatitudeSigmaFilter::getPointDescription() const {
      return "have <i>a priori</i> surface point latitude sigmas which are " +
          descriptionSuffix();
    }
  }
}

