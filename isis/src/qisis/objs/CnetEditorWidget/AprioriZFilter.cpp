#include "IsisDebug.h"

#include "AprioriZFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  AprioriZFilter::AprioriZFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AprioriZFilter::AprioriZFilter(const AprioriZFilter &other)
        : AbstractNumberFilter(other) {
  }


  AprioriZFilter::~AprioriZFilter() {
  }


  bool AprioriZFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AprioriZFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetZ().kilometers());
  }


  bool AprioriZFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AprioriZFilter::clone() const {
    return new AprioriZFilter(*this);
  }


  QString AprioriZFilter::getImageDescription() const {
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


  QString AprioriZFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point radii which are " +
        descriptionSuffix();
  }
}
