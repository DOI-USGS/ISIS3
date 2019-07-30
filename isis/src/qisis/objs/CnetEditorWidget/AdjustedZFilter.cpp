#include "IsisDebug.h"

#include "AdjustedZFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  AdjustedZFilter::AdjustedZFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedZFilter::AdjustedZFilter(const AdjustedZFilter &other)
        : AbstractNumberFilter(other) {
  }


  AdjustedZFilter::~AdjustedZFilter() {
  }


  bool AdjustedZFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AdjustedZFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAdjustedSurfacePoint().GetZ().kilometers());
  }


  bool AdjustedZFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedZFilter::clone() const {
    return new AdjustedZFilter(*this);
  }


  QString AdjustedZFilter::getImageDescription() const {
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


  QString AdjustedZFilter::getPointDescription() const {
    return "have adjusted surface point radii which are " +
        descriptionSuffix();
  }
}
