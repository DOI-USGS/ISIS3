#include "IsisDebug.h"

#include "AdjustedXFilter.h"

#include <QPair>
#include <QString>

#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  AdjustedXFilter::AdjustedXFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedXFilter::AdjustedXFilter(
        const AdjustedXFilter &other) : AbstractNumberFilter(other) {
  }


  AdjustedXFilter::~AdjustedXFilter() {
  }


  bool AdjustedXFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AdjustedXFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAdjustedSurfacePoint().GetX().kilometers());
  }


  bool AdjustedXFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedXFilter::clone() const {
    return new AdjustedXFilter(*this);
  }


  QString AdjustedXFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point X "
          "which is ";
    else
      description += "points that have adjusted surface point Xs "
          "which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedXFilter::getPointDescription() const {
    return "have adjusted surface point Xs which are " +
        descriptionSuffix();
  }
}
