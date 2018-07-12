#include "IsisDebug.h"

#include "SampleShiftFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  SampleShiftFilter::SampleShiftFilter(
    AbstractFilter::FilterEffectivenessFlag flag,
    int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  SampleShiftFilter::SampleShiftFilter(const SampleShiftFilter &other)
        : AbstractNumberFilter(other) {
  }


  SampleShiftFilter::~SampleShiftFilter() {
  }


  bool SampleShiftFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromMeasureFilter(imageAndNet);
  }


  bool SampleShiftFilter::evaluate(const ControlPoint *point) const {
    return evaluatePointFromMeasureFilter(point);
  }


  bool SampleShiftFilter::evaluate(const ControlMeasure *measure) const {
    return AbstractNumberFilter::evaluate(measure->GetSampleShift());
  }


  AbstractFilter *SampleShiftFilter::clone() const {
    return new SampleShiftFilter(*this);
  }


  QString SampleShiftFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1) {
      description += "measure that has a sample shift which is ";
    }
    else {
      description += "measures that have sample shifts which are ";
    }

    description += descriptionSuffix();
    return description;
  }


  QString SampleShiftFilter::getPointDescription() const {
    return getImageDescription();
  }


  QString SampleShiftFilter::getMeasureDescription() const {
    return "have sample shifts which are " + descriptionSuffix();
  }
}
