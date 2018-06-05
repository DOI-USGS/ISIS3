#include "IsisDebug.h"

#include "LineFilter.h"

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"


namespace Isis {
  LineFilter::LineFilter(
    AbstractFilter::FilterEffectivenessFlag flag,
    ControlNet *network,
    int minimumForSuccess) : AbstractNumberFilter(flag, network, minimumForSuccess) {
  }


  LineFilter::LineFilter(const LineFilter &other)
    : AbstractNumberFilter(other) {
  }


  LineFilter::~LineFilter() {
  }


  bool LineFilter::evaluate(const QString *imageSerial) const {
    return evaluateImageFromMeasureFilter(imageSerial);
  }


  bool LineFilter::evaluate(const ControlPoint *point) const {
    return evaluatePointFromMeasureFilter(point);
  }


  bool LineFilter::evaluate(const ControlMeasure *measure) const {
    return AbstractNumberFilter::evaluate(measure->GetLine());
  }


  AbstractFilter *LineFilter::clone() const {
    return new LineFilter(*this);
  }


  QString LineFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "measure that has a line which is ";
    else
      description += "measures that have lines which are ";

    description += descriptionSuffix();
    return description;
  }


  QString LineFilter::getPointDescription() const {
    return getImageDescription();
  }


  QString LineFilter::getMeasureDescription() const {
    return "have lines which are " + descriptionSuffix();
  }
}
