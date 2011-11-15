#include "IsisDebug.h"

#include "CubeSerialNumberFilter.h"

#include <QString>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "iString.h"


namespace Isis
{
  namespace CnetViz
  {
    CubeSerialNumberFilter::CubeSerialNumberFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractStringFilter(flag, minimumForSuccess)
    {
    }


    CubeSerialNumberFilter::CubeSerialNumberFilter(
        const CubeSerialNumberFilter & other): AbstractStringFilter(other)
    {
    }


    CubeSerialNumberFilter::~CubeSerialNumberFilter()
    {
    }


    bool CubeSerialNumberFilter::evaluate(
        const ControlCubeGraphNode * node) const
    {
      return AbstractStringFilter::evaluate((QString) node->getSerialNumber());
    }


    bool CubeSerialNumberFilter::evaluate(const ControlPoint * point) const
    {
      return evaluatePointFromMeasureFilter(point);
    }


    bool CubeSerialNumberFilter::evaluate(const ControlMeasure * measure) const
    {
      return AbstractStringFilter::evaluate(
          (QString) measure->GetCubeSerialNumber());
    }


    AbstractFilter * CubeSerialNumberFilter::clone() const
    {
      return new CubeSerialNumberFilter(*this);
    }


    QString CubeSerialNumberFilter::getImageDescription() const
    {
      return getMeasureDescription();
    }


    QString CubeSerialNumberFilter::getPointDescription() const
    {
      QString description = AbstractFilter::getImageDescription();

      if (getMinForSuccess() == 1)
        description += "measure with it's cube serial number ";
      else
        description += "measures with cube serial numbers ";

      description += descriptionSuffix();
      return description;
    }


    QString CubeSerialNumberFilter::getMeasureDescription() const
    {
      return "have cube serial numbers " + descriptionSuffix();
    }
  }
}
