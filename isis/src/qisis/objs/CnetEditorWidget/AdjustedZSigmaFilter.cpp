#include "IsisDebug.h"

#include "AdjustedZSigmaFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  AdjustedZSigmaFilter::AdjustedZSigmaFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedZSigmaFilter::AdjustedZSigmaFilter(const AdjustedZSigmaFilter &other)
        : AbstractNumberFilter(other) {
  }


  AdjustedZSigmaFilter::~AdjustedZSigmaFilter() {
  }


  bool AdjustedZSigmaFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AdjustedZSigmaFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAdjustedSurfacePoint().GetZSigma().meters());
  }


  bool AdjustedZSigmaFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedZSigmaFilter::clone() const {
    return new AdjustedZSigmaFilter(*this);
  }


  QString AdjustedZSigmaFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point Z sigma "
          "which is ";
    else
      description += "points that have adjusted surface point Z sigmas "
          "which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedZSigmaFilter::getPointDescription() const {
    return "have adjusted surface point Z sigmas which are " +
        descriptionSuffix();
  }
}
