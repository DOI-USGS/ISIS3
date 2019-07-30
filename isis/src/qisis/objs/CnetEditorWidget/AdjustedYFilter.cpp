#include "IsisDebug.h"

#include "AdjustedYFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  AdjustedYFilter::AdjustedYFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess)
        : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AdjustedYFilter::AdjustedYFilter(const AdjustedYFilter &other)
        : AbstractNumberFilter(other) {
  }


  AdjustedYFilter::~AdjustedYFilter() {
  }


  bool AdjustedYFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AdjustedYFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAdjustedSurfacePoint().GetY().kilometers());
  }


  bool AdjustedYFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AdjustedYFilter::clone() const {
    return new AdjustedYFilter(*this);
  }


  QString AdjustedYFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an adjusted surface point longitude "
          "which is ";
    else
      description += "points that have adjusted surface point longitudes "
          "which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AdjustedYFilter::getPointDescription() const {
    return "have adjusted surface point longitudes which are " +
        descriptionSuffix();
  }
}
