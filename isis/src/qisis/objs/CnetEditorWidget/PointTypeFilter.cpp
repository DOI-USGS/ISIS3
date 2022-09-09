/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "PointTypeFilter.h"

#include <QPair>
#include <QString>
#include <QStringList>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "IString.h"


namespace Isis {
  PointTypeFilter::PointTypeFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractMultipleChoiceFilter(flag, minimumForSuccess) {
    QStringList options;
    options << "Fixed" << "Constrained" << "Free";
    createWidget(options);
  }


  PointTypeFilter::PointTypeFilter(const PointTypeFilter &other)
        : AbstractMultipleChoiceFilter(other) {
  }


  PointTypeFilter::~PointTypeFilter() {
  }


  bool PointTypeFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool PointTypeFilter::evaluate(const ControlPoint *point) const {
    return ((QString) point->GetPointTypeString() == getCurrentChoice()) ^
           !inclusive();
  }


  bool PointTypeFilter::evaluate(const ControlMeasure *) const {
    return true;
  }


  AbstractFilter *PointTypeFilter::clone() const {
    return new PointTypeFilter(*this);
  }


  QString PointTypeFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription() + "point";

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


  QString PointTypeFilter::getPointDescription() const {
    QString description = "are ";

    if (!inclusive()) {
      description += "not ";
    }

    description += "of type " + getCurrentChoice();

    return description;
  }
}
