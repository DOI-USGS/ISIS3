#include "IsisDebug.h"

#include "MeasureIgnoredFilter.h"

#include <iostream>

#include <QHBoxLayout>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"


namespace Isis {
  MeasureIgnoredFilter::MeasureIgnoredFilter(
    AbstractFilter::FilterEffectivenessFlag flag, int minimumForSuccess) :
    AbstractFilter(flag, minimumForSuccess) {
  }


  MeasureIgnoredFilter::~MeasureIgnoredFilter() {
  }


  bool MeasureIgnoredFilter::evaluate(const ControlCubeGraphNode *node) const {
    return AbstractFilter::evaluateImageFromMeasureFilter(node);
  }


  bool MeasureIgnoredFilter::evaluate(const ControlPoint *point) const {
    return AbstractFilter::evaluatePointFromMeasureFilter(point);
  }


  bool MeasureIgnoredFilter::evaluate(const ControlMeasure *measure) const {
    return AbstractFilter::evaluate(measure, &ControlMeasure::IsIgnored);
  }


  AbstractFilter *MeasureIgnoredFilter::clone() const {
    return new MeasureIgnoredFilter(*this);
  }


  QString MeasureIgnoredFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "measure that is ";
    else
      description += "measures that are ";

    if (inclusive())
      description += "ignored";
    else
      description += "not ignored";

    return description;
  }


  QString MeasureIgnoredFilter::getPointDescription() const {
    return getImageDescription();
  }


  QString MeasureIgnoredFilter::getMeasureDescription() const {
    QString description = "are ";

    if (inclusive())
      description += "ignored";
    else
      description += "not ignored";

    return description;
  }
}
