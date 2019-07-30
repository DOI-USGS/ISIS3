#include "IsisDebug.h"

#include "AprioriXSigmaFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  AprioriXSigmaFilter::AprioriXSigmaFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AprioriXSigmaFilter::AprioriXSigmaFilter(const AprioriXSigmaFilter &other)
        : AbstractNumberFilter(other) {
  }


  AprioriXSigmaFilter::~AprioriXSigmaFilter() {
  }


  bool AprioriXSigmaFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AprioriXSigmaFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetXSigma().meters());
  }


  bool AprioriXSigmaFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AprioriXSigmaFilter::clone() const {
    return new AprioriXSigmaFilter(*this);
  }


  QString AprioriXSigmaFilter::getImageDescription() const {
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


  QString AprioriXSigmaFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point latitude sigmas which are " +
          descriptionSuffix();
  }
}
