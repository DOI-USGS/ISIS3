/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
