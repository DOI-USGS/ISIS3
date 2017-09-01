#include "IsisDebug.h"

#include "LineShiftFilter.h"

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"


namespace Isis {
  LineShiftFilter::LineShiftFilter(
    AbstractFilter::FilterEffectivenessFlag flag,
    int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  LineShiftFilter::LineShiftFilter(const LineShiftFilter &other)
    : AbstractNumberFilter(other) {
  }


  LineShiftFilter::~LineShiftFilter() {
  }


  bool LineShiftFilter::evaluate(const ControlCubeGraphNode *node) const {
    return evaluateImageFromMeasureFilter(node);
  }


  bool LineShiftFilter::evaluate(const ControlPoint *point) const {
    return evaluatePointFromMeasureFilter(point);
  }


  bool LineShiftFilter::evaluate(const ControlMeasure *measure) const {
    return AbstractNumberFilter::evaluate(measure->GetLineShift());
  }


  AbstractFilter *LineShiftFilter::clone() const {
    return new LineShiftFilter(*this);
  }


  QString LineShiftFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "measure that has a line shift which is ";
    else
      description += "measures that have line shifts which are ";

    description += descriptionSuffix();
    return description;
  }


  QString LineShiftFilter::getPointDescription() const {
    return getImageDescription();
  }


  QString LineShiftFilter::getMeasureDescription() const {
    return "have line shifts which are " + descriptionSuffix();
  }
}

