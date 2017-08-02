#include "IsisDebug.h"

#include "SampleFilter.h"

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"


namespace Isis {
  SampleFilter::SampleFilter(
    AbstractFilter::FilterEffectivenessFlag flag,
    int minimumForSuccess) :
    AbstractNumberFilter(flag, minimumForSuccess) {
  }


  SampleFilter::SampleFilter(
    const SampleFilter &other) : AbstractNumberFilter(other) {
  }


  SampleFilter::~SampleFilter() {
  }


  bool SampleFilter::evaluate(const ControlCubeGraphNode *node) const {
    return evaluateImageFromMeasureFilter(node);
  }


  bool SampleFilter::evaluate(const ControlPoint *point) const {
    return evaluatePointFromMeasureFilter(point);
  }


  bool SampleFilter::evaluate(const ControlMeasure *measure) const {
    return AbstractNumberFilter::evaluate(measure->GetSample());
  }


  AbstractFilter *SampleFilter::clone() const {
    return new SampleFilter(*this);
  }


  QString SampleFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1)
      description += "measure that has a sample which is ";
    else
      description += "measures that have samples which are ";

    description += descriptionSuffix();
    return description;
  }


  QString SampleFilter::getPointDescription() const {
    return getImageDescription();
  }


  QString SampleFilter::getMeasureDescription() const {
    return "have samples which are " + descriptionSuffix();
  }
}

