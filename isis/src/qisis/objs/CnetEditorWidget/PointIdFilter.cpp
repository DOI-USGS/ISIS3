#include "IsisDebug.h"

#include "PointIdFilter.h"

#include <QString>

#include "ControlCubeGraphNode.h"
#include "ControlPoint.h"
#include "IString.h"


namespace Isis {
  PointIdFilter::PointIdFilter(AbstractFilter::FilterEffectivenessFlag flag,
      ControlNet *network,
      int minimumForSuccess) : AbstractStringFilter(flag, network, minimumForSuccess) {
  }


  PointIdFilter::PointIdFilter(const PointIdFilter &other)
    : AbstractStringFilter(other) {
  }


  PointIdFilter::~PointIdFilter() {
  }


  bool PointIdFilter::evaluate(const QString *imageSerial) const {
    return evaluateImageFromPointFilter(imageSerial);
  }


  bool PointIdFilter::evaluate(const ControlPoint *point) const {
    return AbstractStringFilter::evaluate((QString) point->GetId());
  }


  bool PointIdFilter::evaluate(const ControlMeasure *) const {
    return true;
  }


  AbstractFilter *PointIdFilter::clone() const {
    return new PointIdFilter(*this);
  }


  QString PointIdFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();

    if (getMinForSuccess() == 1)
      description += "point with it's ID ";
    else
      description += "points with IDs ";

    description += descriptionSuffix();
    return description;
  }


  QString PointIdFilter::getPointDescription() const {
    return "have IDs " + descriptionSuffix();
  }
}
