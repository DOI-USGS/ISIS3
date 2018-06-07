#include "IsisDebug.h"

#include "ResidualMagnitudeFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  ResidualMagnitudeFilter::ResidualMagnitudeFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  ResidualMagnitudeFilter::ResidualMagnitudeFilter(const ResidualMagnitudeFilter &other)
        : AbstractNumberFilter(other) {
  }


  ResidualMagnitudeFilter::~ResidualMagnitudeFilter() {
  }


  bool ResidualMagnitudeFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromMeasureFilter(imageAndNet);
  }


  bool ResidualMagnitudeFilter::evaluate(const ControlPoint *point) const {
    return evaluatePointFromMeasureFilter(point);
  }


  bool ResidualMagnitudeFilter::evaluate(const ControlMeasure *measure) const {
    return AbstractNumberFilter::evaluate(measure->GetResidualMagnitude());
  }


  AbstractFilter *ResidualMagnitudeFilter::clone() const {
    return new ResidualMagnitudeFilter(*this);
  }


  QString ResidualMagnitudeFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1) {
      description += "measure that has a residual magnitude which is ";
    }
    else {
      description += "measures that have residual magnitudes which are ";
    }

    description += descriptionSuffix();
    return description;
  }


  QString ResidualMagnitudeFilter::getPointDescription() const {
    return getImageDescription();
  }


  QString ResidualMagnitudeFilter::getMeasureDescription() const {
    return "that have residual magnitudes which are " + descriptionSuffix();
  }
}
