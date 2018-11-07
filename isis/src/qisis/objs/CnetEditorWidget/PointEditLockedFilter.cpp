#include "IsisDebug.h"

#include "PointEditLockedFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  PointEditLockedFilter::PointEditLockedFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractFilter(flag, minimumForSuccess) {
  }


  PointEditLockedFilter::PointEditLockedFilter(const AbstractFilter &other)
        : AbstractFilter(other) {
  }


  PointEditLockedFilter::~PointEditLockedFilter() {
  }


  bool PointEditLockedFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return AbstractFilter::evaluateImageFromPointFilter(imageAndNet);
  }


  bool PointEditLockedFilter::evaluate(const ControlPoint *point) const {
    return AbstractFilter::evaluate(point, &ControlPoint::IsEditLocked);
  }


  bool PointEditLockedFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *PointEditLockedFilter::clone() const {
    return new PointEditLockedFilter(*this);
  }


  QString PointEditLockedFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();

    if (getMinForSuccess() == 1) {
      description += "point that is ";
    }
    else {
      description += "points that are ";
    }

    if (inclusive()) {
      description += "edit locked";
    }
    else {
      description += "not edit locked";
    }

    return description;
  }


  QString PointEditLockedFilter::getPointDescription() const {
    QString description = "are ";

    if (inclusive()) {
      description += "edit locked";
    }
    else {
      description += "not edit locked";
    }

    return description;
  }
}
