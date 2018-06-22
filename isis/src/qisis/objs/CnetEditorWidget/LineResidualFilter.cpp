#include "IsisDebug.h"

#include "LineResidualFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


namespace Isis {
  LineResidualFilter::LineResidualFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess) {
  }


  LineResidualFilter::LineResidualFilter(const LineResidualFilter &other)
        : AbstractNumberFilter(other) {
  }


  LineResidualFilter::~LineResidualFilter() {
  }


  bool LineResidualFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromMeasureFilter(imageAndNet);
  }


  bool LineResidualFilter::evaluate(const ControlPoint *point) const {
    return evaluatePointFromMeasureFilter(point);
  }


  bool LineResidualFilter::evaluate(const ControlMeasure *measure) const {
    return AbstractNumberFilter::evaluate(measure->GetLineResidual());
  }


  AbstractFilter *LineResidualFilter::clone() const {
    return new LineResidualFilter(*this);
  }


  QString LineResidualFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();

    if (getMinForSuccess() == 1) {
      description += "measure that has a line residual which is ";
    }
    else {
      description += "measures that have line residuals which are ";
    }

    description += descriptionSuffix();
    return description;
  }


  QString LineResidualFilter::getPointDescription() const {
    return getImageDescription();
  }


  QString LineResidualFilter::getMeasureDescription() const {
    return "have line residuals which are " + descriptionSuffix();
  }
}
