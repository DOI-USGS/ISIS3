#include "IsisDebug.h"

#include "AprioriXFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  AprioriXFilter::AprioriXFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  AprioriXFilter::AprioriXFilter(const AprioriXFilter &other)
        : AbstractNumberFilter(other) {
  }


  AprioriXFilter::~AprioriXFilter() {
  }


  bool AprioriXFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool AprioriXFilter::evaluate(const ControlPoint *point) const {
    return AbstractNumberFilter::evaluate(
          point->GetAprioriSurfacePoint().GetX().kilometers());
  }


  bool AprioriXFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *AprioriXFilter::clone() const {
    return new AprioriXFilter(*this);
  }


  QString AprioriXFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "point that has an <i>a priori</i> surface point "
          "latitude which is ";
    else
      description += "points that have <i>a priori</i> surface point "
          "latitudes which are ";

    description += descriptionSuffix();
    return description;
  }


  QString AprioriXFilter::getPointDescription() const {
    return "have <i>a priori</i> surface point latitudes which are " +
        descriptionSuffix();
  }
}
