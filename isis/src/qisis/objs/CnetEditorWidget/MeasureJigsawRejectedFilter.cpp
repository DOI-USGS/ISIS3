#include "IsisDebug.h"

#include "MeasureJigsawRejectedFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"

using std::cerr;


namespace Isis {
  MeasureJigsawRejectedFilter::MeasureJigsawRejectedFilter(
        AbstractFilter::FilterEffectivenessFlag flag, int minimumForSuccess) :
        AbstractFilter(flag, minimumForSuccess) {
  }


  MeasureJigsawRejectedFilter::~MeasureJigsawRejectedFilter() {
  }


  bool MeasureJigsawRejectedFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return AbstractFilter::evaluateImageFromMeasureFilter(imageAndNet);
  }


  bool MeasureJigsawRejectedFilter::evaluate(const ControlPoint *point) const {
    return AbstractFilter::evaluatePointFromMeasureFilter(point);
  }


  bool MeasureJigsawRejectedFilter::evaluate(const ControlMeasure *measure) const {
    return AbstractFilter::evaluate(measure, &ControlMeasure::IsRejected);
  }


  AbstractFilter *MeasureJigsawRejectedFilter::clone() const {
    return new MeasureJigsawRejectedFilter(*this);
  }


  QString MeasureJigsawRejectedFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();
    if (getMinForSuccess() == 1) {
      description += "measure that is ";
    }
    else {
      description += "measures that are ";
    }

    if (inclusive()) {
      description += "jigsaw rejected";
    }
    else {
      description += "not jigsaw rejected";
    }

    return description;
  }


  QString MeasureJigsawRejectedFilter::getPointDescription() const {
    return getImageDescription();
  }


  QString MeasureJigsawRejectedFilter::getMeasureDescription() const {
    QString description = "are ";

    if (inclusive()) {
      description += "jigsaw rejected";
    }
    else {
      description += "not jigsaw rejected";
    }

    return description;
  }
}
