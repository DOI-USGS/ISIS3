#include "IsisDebug.h"

#include "SampleResidualFilter.h"

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"


namespace Isis
{
  namespace CnetViz
  {
    SampleResidualFilter::SampleResidualFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) :
        AbstractNumberFilter(flag, minimumForSuccess)
    {
    }


    SampleResidualFilter::SampleResidualFilter(
        const SampleResidualFilter & other) : AbstractNumberFilter(other)
    {
    }


    SampleResidualFilter::~SampleResidualFilter()
    {
    }


    bool SampleResidualFilter::evaluate(const ControlCubeGraphNode * node) const
    {
      return evaluateImageFromMeasureFilter(node);
    }


    bool SampleResidualFilter::evaluate(const ControlPoint * point) const
    {
      return evaluatePointFromMeasureFilter(point);
    }


    bool SampleResidualFilter::evaluate(const ControlMeasure * measure) const
    {
      return AbstractNumberFilter::evaluate(measure->GetSampleResidual());
    }


    AbstractFilter * SampleResidualFilter::clone() const
    {
      return new SampleResidualFilter(*this);
    }


    QString SampleResidualFilter::getImageDescription() const
    {
      QString description = AbstractFilter::getImageDescription();
      if (getMinForSuccess() == 1)
        description += "measure that has a sample residual which is ";
      else
        description += "measures that have sample residuals which are ";

      description += descriptionSuffix();
      return description;
    }


    QString SampleResidualFilter::getPointDescription() const
    {
      return getImageDescription();
    }


    QString SampleResidualFilter::getMeasureDescription() const
    {
      return "that have sample residuals which are " + descriptionSuffix();
    }
  }
}
