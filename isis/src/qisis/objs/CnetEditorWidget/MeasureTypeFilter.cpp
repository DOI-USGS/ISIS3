#include "IsisDebug.h"

#include "MeasureTypeFilter.h"

#include <QString>
#include <QStringList>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "IString.h"


namespace Isis {
  MeasureTypeFilter::MeasureTypeFilter(
    AbstractFilter::FilterEffectivenessFlag flag, ControlNet *network, int minimumForSuccess) :
    AbstractMultipleChoiceFilter(flag, network, minimumForSuccess) {
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


  bool MeasureTypeFilter::evaluate(const QString *imageSerial) const {
    return evaluateImageFromMeasureFilter(imageSerial);
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

    if (getMinForSuccess() != 1)
      description += "s ";
    else
      description += " ";

    description += "that ";

    if (getMinForSuccess() == 1)
      description += "is ";
    else
      description += "are ";

    if (!inclusive())
      description += "not ";

    description += " of type " + getCurrentChoice();

    return description;
  }


  QString MeasureTypeFilter::getMeasureDescription() const {
    QString description = "are ";

    if (!inclusive())
      description += "not ";

    description += "of type " + getCurrentChoice();

    return description;
  }


  QString MeasureTypeFilter::getPointDescription() const {
    return getImageDescription();
  }
}
