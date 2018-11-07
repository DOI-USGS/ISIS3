#include "IsisDebug.h"

#include "PointIgnoredFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  PointIgnoredFilter::PointIgnoredFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractFilter(flag, minimumForSuccess) {
  }


  PointIgnoredFilter::PointIgnoredFilter(const AbstractFilter &other) : AbstractFilter(other) {
  }


  PointIgnoredFilter::~PointIgnoredFilter() {
  }


  bool PointIgnoredFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return AbstractFilter::evaluateImageFromPointFilter(imageAndNet);
  }


  bool PointIgnoredFilter::evaluate(const ControlPoint *point) const {
    return AbstractFilter::evaluate(point, &ControlPoint::IsIgnored);
  }


  bool PointIgnoredFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *PointIgnoredFilter::clone() const {
    return new PointIgnoredFilter(*this);
  }


  QString PointIgnoredFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();

    if (getMinForSuccess() == 1) {
      description += "point that is ";
    }
    else {
      description += "points that are ";
    }

    if (inclusive()) {
      description += "ignored";
    }
    else {
      description += "not ignored";
    }

    return description;
  }


  QString PointIgnoredFilter::getPointDescription() const {
    QString description = "are ";

    if (inclusive()) {
      description += "ignored";
    }
    else {
      description += "not ignored";
    }

    return description;
  }
}
