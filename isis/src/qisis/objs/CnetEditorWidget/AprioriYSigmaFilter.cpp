#include "IsisDebug.h"

#include "AprioriYSigmaFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Longitude.h"


namespace Isis {
  AprioriYSigmaFilter::AprioriYSigmaFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AprioriYSigmaFilter::AprioriYSigmaFilter(
        const AprioriYSigmaFilter &other) : AbstractNumberFilter(other) {
  }


  AprioriYSigmaFilter::~AprioriYSigmaFilter() {
  }


  bool AprioriYSigmaFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AprioriYSigmaFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetYSigma().meters());
  }


  bool AprioriYSigmaFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AprioriYSigmaFilter::clone() const {
    return new AprioriYSigmaFilter(*this);
  }


  QString AprioriYSigmaFilter::getImageDescription() const {
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


  QString AprioriYSigmaFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point longitude sigmas which are " +
        descriptionSuffix();
  }
}
