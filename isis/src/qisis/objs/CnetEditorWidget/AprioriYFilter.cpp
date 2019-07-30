#include "IsisDebug.h"

#include "AprioriYFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Longitude.h"


namespace Isis {
  AprioriYFilter::AprioriYFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AprioriYFilter::AprioriYFilter(const AprioriYFilter &other)
        : AbstractNumberFilter(other) {
  }


  AprioriYFilter::~AprioriYFilter() {
  }


  bool AprioriYFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AprioriYFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetY().kilometers());
  }


  bool AprioriYFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AprioriYFilter::clone() const {
    return new AprioriYFilter(*this);
  }


  QString AprioriYFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an <i>a priori</i> surface point "
          "longitude which is ";
    else
      description += "points that have <i>a priori</i> surface point "
          "longitudes which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AprioriYFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point longitudes which are " +
        descriptionSuffix();
  }
}
