#include "IsisDebug.h"

#include "ResidualMagnitudeFilter.h"

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"


namespace Isis
{
  namespace CnetViz
  {
    ResidualMagnitudeFilter::ResidualMagnitudeFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractNumberFilter(flag, minimumForSuccess)
    {
    }


    ResidualMagnitudeFilter::ResidualMagnitudeFilter(
        const ResidualMagnitudeFilter & other) : AbstractNumberFilter(other)
    {
    }


    ResidualMagnitudeFilter::~ResidualMagnitudeFilter()
    {
    }


    bool ResidualMagnitudeFilter::evaluate(
        const ControlCubeGraphNode * node) const
    {
      return evaluateImageFromMeasureFilter(node);
    }


    bool ResidualMagnitudeFilter::evaluate(const ControlPoint * point) const
    {
      return evaluatePointFromMeasureFilter(point);
    }


    bool ResidualMagnitudeFilter::evaluate(const ControlMeasure * measure) const
    {
      return AbstractNumberFilter::evaluate(measure->GetResidualMagnitude());
    }


    AbstractFilter * ResidualMagnitudeFilter::clone() const
    {
      return new ResidualMagnitudeFilter(*this);
    }


    QString ResidualMagnitudeFilter::getImageDescription() const
    {
      QString description = AbstractFilter::getImageDescription();
      if (getMinForSuccess() == 1)
        description += "measure that has a residual magnitude which is ";
      else
        description += "measures that have residual magnitudes which are ";

      description += descriptionSuffix();
      return description;
    }


    QString ResidualMagnitudeFilter::getPointDescription() const
    {
      return getImageDescription();
    }


    QString ResidualMagnitudeFilter::getMeasureDescription() const
    {
      return "that have residual magnitudes which are " + descriptionSuffix();
    }
  }
}
