#include "IsisDebug.h"

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
