#include "IsisDebug.h"

#include "PointIdFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "IString.h"


namespace Isis {
  PointIdFilter::PointIdFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractStringFilter(flag, minimumForSuccess) {
  }


  PointIdFilter::PointIdFilter(const PointIdFilter &other) : AbstractStringFilter(other) {
  }


  PointIdFilter::~PointIdFilter() {
  }


  bool PointIdFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool PointIdFilter::evaluate(const ControlPoint *point) const {
    return AbstractStringFilter::evaluate((QString) point->GetId());
  }


  bool PointIdFilter::evaluate(const ControlMeasure *) const {
    return true;
  }


  AbstractFilter *PointIdFilter::clone() const {
    return new PointIdFilter(*this);
  }


  QString PointIdFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();

    if (getMinForSuccess() == 1) {
      description += "point with it's ID ";
    }
    else {
      description += "points with IDs ";
    }

    description += descriptionSuffix();
    return description;
  }


  QString PointIdFilter::getPointDescription() const {
    return "have IDs " + descriptionSuffix();
  }
}
