#include "IsisDebug.h"

#include "APrioriZFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  APrioriZFilter::APrioriZFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  APrioriZFilter::APrioriZFilter(const APrioriZFilter &other)
        : AbstractNumberFilter(other) {
  }


  APrioriZFilter::~APrioriZFilter() {
  }


  bool APrioriZFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool APrioriZFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetZ().kilometers());
  }


  bool APrioriZFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *APrioriZFilter::clone() const {
    return new APrioriZFilter(*this);
  }


  QString APrioriZFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an <i>a priori</i> surface point "
          "Z which is ";
    else
      description += "points that have <i>a priori</i> surface point "
          "Zs which are ";

    description += descriptionSuffix();
    return description;
  }


  QString APrioriZFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point Zs which are " +
        descriptionSuffix();
  }
}
