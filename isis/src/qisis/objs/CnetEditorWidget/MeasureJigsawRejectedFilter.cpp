/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
