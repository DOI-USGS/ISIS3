#include "IsisDebug.h"

#include "ImageIdFilter.h"

#include <QString>

#include "CnetDisplayProperties.h"
#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "IString.h"


namespace Isis {
  ImageIdFilter::ImageIdFilter(
    AbstractFilter::FilterEffectivenessFlag flag,
    ControlNet *network, int minimumForSuccess) : AbstractStringFilter(flag, network, minimumForSuccess) {
  }


  ImageIdFilter::ImageIdFilter(
    const ImageIdFilter &other): AbstractStringFilter(other) {
  }


  ImageIdFilter::~ImageIdFilter() {
  }


  bool ImageIdFilter::evaluate(const QString *imageSerial) const {
    return AbstractStringFilter::evaluate(
        CnetDisplayProperties::getInstance()->getImageName(*imageSerial));
  }


  bool ImageIdFilter::evaluate(const ControlPoint *point) const {
    return evaluatePointFromMeasureFilter(point);
  }


  bool ImageIdFilter::evaluate(const ControlMeasure *measure) const {
    return AbstractStringFilter::evaluate(
        CnetDisplayProperties::getInstance()->getImageName(
            (QString) measure->GetCubeSerialNumber()));
  }


  AbstractFilter *ImageIdFilter::clone() const {
    return new ImageIdFilter(*this);
  }


  QString ImageIdFilter::getImageDescription() const {
    return getMeasureDescription();
  }


  QString ImageIdFilter::getPointDescription() const {
    QString description = AbstractFilter::getImageDescription();

    if (getMinForSuccess() == 1)
      description += "measure with it's image ID ";
    else
      description += "measures with image IDs ";

    description += descriptionSuffix();
    return description;
  }


  QString ImageIdFilter::getMeasureDescription() const {
    return "have image IDs " + descriptionSuffix();
  }
}
