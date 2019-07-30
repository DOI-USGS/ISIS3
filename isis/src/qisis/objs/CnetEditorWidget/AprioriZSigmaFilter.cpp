#include "IsisDebug.h"

#include "AprioriZSigmaFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  AprioriZSigmaFilter::AprioriZSigmaFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AprioriZSigmaFilter::AprioriZSigmaFilter(const AprioriZSigmaFilter &other)
        : AbstractNumberFilter(other) {
  }


  AprioriZSigmaFilter::~AprioriZSigmaFilter() {
  }


  bool AprioriZSigmaFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AprioriZSigmaFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetZSigma().meters());
  }


  bool AprioriZSigmaFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AprioriZSigmaFilter::clone() const {
    return new AprioriZSigmaFilter(*this);
  }


  QString AprioriZSigmaFilter::getImageDescription() const {
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


  QString AprioriZSigmaFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point radius sigmas which are " +
        descriptionSuffix();
  }
}
