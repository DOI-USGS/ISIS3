#include "IsisDebug.h"

#include "ImageIdFilter.h"

#include <QPair>
#include <QString>

#include "CnetDisplayProperties.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "IString.h"


namespace Isis {
  ImageIdFilter::ImageIdFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractStringFilter(flag, minimumForSuccess) {
  }


  ImageIdFilter::ImageIdFilter(const ImageIdFilter &other): AbstractStringFilter(other) {
  }


  ImageIdFilter::~ImageIdFilter() {
  }


  bool ImageIdFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return AbstractStringFilter::evaluate(
          CnetDisplayProperties::getInstance()->getImageName(imageAndNet->first));
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
