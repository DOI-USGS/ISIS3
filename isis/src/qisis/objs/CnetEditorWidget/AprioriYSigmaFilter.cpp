#include "IsisDebug.h"

#include "APrioriYSigmaFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Longitude.h"


namespace Isis {
  APrioriYSigmaFilter::APrioriYSigmaFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  APrioriYSigmaFilter::APrioriYSigmaFilter(
        const APrioriYSigmaFilter &other) : AbstractNumberFilter(other) {
  }


  APrioriYSigmaFilter::~APrioriYSigmaFilter() {
  }


  bool APrioriYSigmaFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool APrioriYSigmaFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetYSigma().meters());
  }


  bool APrioriYSigmaFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *APrioriYSigmaFilter::clone() const {
    return new APrioriYSigmaFilter(*this);
  }


  QString APrioriYSigmaFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an <i>a priori</i> surface point "
          "Y sigma which is ";
    else
      description += "points that have <i>a priori</i> surface point "
          "Y sigmas which are ";

    description += descriptionSuffix();
    return description;
  }


  QString APrioriYSigmaFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point Y sigmas which are " +
        descriptionSuffix();
  }
}
