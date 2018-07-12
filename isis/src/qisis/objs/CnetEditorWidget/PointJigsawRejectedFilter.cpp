#include "IsisDebug.h"

#include "PointJigsawRejectedFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"


using std::cerr;


namespace Isis {
  PointJigsawRejectedFilter::PointJigsawRejectedFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractFilter(flag, minimumForSuccess) {
  }


  PointJigsawRejectedFilter::PointJigsawRejectedFilter(const AbstractFilter &other)
        : AbstractFilter(other) {
  }


  PointJigsawRejectedFilter::~PointJigsawRejectedFilter() {
  }


  bool PointJigsawRejectedFilter::evaluate(
        const QPair<QString, ControlNet *> *imageAndNet) const {
    return AbstractFilter::evaluateImageFromPointFilter(imageAndNet);
  }


  bool PointJigsawRejectedFilter::evaluate(const ControlPoint *point) const {
    return AbstractFilter::evaluate(point, &ControlPoint::IsRejected);
  }


  bool PointJigsawRejectedFilter::evaluate(const ControlMeasure *measure) const {
    return true;
  }


  AbstractFilter *PointJigsawRejectedFilter::clone() const {
    return new PointJigsawRejectedFilter(*this);
  }


  QString PointJigsawRejectedFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();

    if (getMinForSuccess() == 1) {
      description += "point that is ";
    }
    else {
      description += "points that are ";
    }

    if (inclusive()) {
      description += "jigsaw rejected";
    }
    else {
      description += "not jigsaw rejected";
    }

    return description;
  }


  QString PointJigsawRejectedFilter::getPointDescription() const {
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
