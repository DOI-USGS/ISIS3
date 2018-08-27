#include "IsisDebug.h"

#include "APrioriRadiusFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  APrioriRadiusFilter::APrioriRadiusFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  APrioriRadiusFilter::APrioriRadiusFilter(const APrioriRadiusFilter &other)
        : AbstractNumberFilter(other) {
  }


  APrioriRadiusFilter::~APrioriRadiusFilter() {
  }


  bool APrioriRadiusFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool APrioriRadiusFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetLocalRadius().meters());
  }


  bool APrioriRadiusFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *APrioriRadiusFilter::clone() const {
    return new APrioriRadiusFilter(*this);
  }


  QString APrioriRadiusFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an <i>a priori</i> surface point "
          "radius which is ";
    else
      description += "points that have <i>a priori</i> surface point "
          "radii which are ";

    description += descriptionSuffix();
    return description;
  }


  QString APrioriRadiusFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point radii which are " +
        descriptionSuffix();
  }
}
