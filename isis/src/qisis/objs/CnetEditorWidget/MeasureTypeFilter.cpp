/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "MeasureTypeFilter.h"

#include <QPair>
#include <QString>
#include <QStringList>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "IString.h"


namespace Isis {
  MeasureTypeFilter::MeasureTypeFilter(
        AbstractFilter::FilterEffectivenessFlag flag, int minimumForSuccess) :
        AbstractMultipleChoiceFilter(flag, minimumForSuccess) {
    QStringList options;
    options << "Candidate" << "Manual" << "RegisteredPixel" <<
        "RegisteredSubPixel";
    createWidget(options);
  }


  MeasureTypeFilter::MeasureTypeFilter(const MeasureTypeFilter &other)
        : AbstractMultipleChoiceFilter(other) {
  }


  MeasureTypeFilter::~MeasureTypeFilter() {
  }


  bool MeasureTypeFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromMeasureFilter(imageAndNet);
  }


  bool MeasureTypeFilter::evaluate(const ControlPoint *point) const {
    return evaluatePointFromMeasureFilter(point);
  }


  bool MeasureTypeFilter::evaluate(const ControlMeasure *measure) const {
    return ((QString) measure->GetMeasureTypeString() == getCurrentChoice()) ^
           !inclusive();
  }


  AbstractFilter *MeasureTypeFilter::clone() const {
    return new MeasureTypeFilter(*this);
  }


  QString MeasureTypeFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription() + "measure";

    if (getMinForSuccess() != 1) {
      description += "s ";
    }
    else {
      description += " ";
    }

    description += "that ";

    if (getMinForSuccess() == 1) {
      description += "is ";
    }
    else {
      description += "are ";
    }

    if (!inclusive()) {
      description += "not ";
    }

    description += " of type " + getCurrentChoice();

    return description;
  }


  QString MeasureTypeFilter::getMeasureDescription() const {
    QString description = "are ";

    if (!inclusive()) {
      description += "not ";
    }

    description += "of type " + getCurrentChoice();

    return description;
  }


  QString MeasureTypeFilter::getPointDescription() const {
    return getImageDescription();
  }
}
