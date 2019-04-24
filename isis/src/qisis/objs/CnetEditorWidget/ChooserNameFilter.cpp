#include "IsisDebug.h"

#include "ChooserNameFilter.h"

#include <QPair>
#include <QString>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "IString.h"


namespace Isis {
  ChooserNameFilter::ChooserNameFilter(
        AbstractFilter::FilterEffectivenessFlag flag,
        int minimumForSuccess) : AbstractStringFilter(flag, minimumForSuccess) {
  }


  ChooserNameFilter::ChooserNameFilter(const ChooserNameFilter &other)
        : AbstractStringFilter(other) {
  }


  ChooserNameFilter::~ChooserNameFilter() {
  }


  bool ChooserNameFilter::evaluate(const QPair<QString, ControlNet *> *imageAndNet) const {
    return evaluateImageFromPointFilter(imageAndNet);
  }


  bool ChooserNameFilter::evaluate(const ControlPoint *point) const {
    return AbstractStringFilter::evaluate((QString) point->GetChooserName());
  }


  bool ChooserNameFilter::evaluate(const ControlMeasure *) const {
    return true;
  }


  AbstractFilter *ChooserNameFilter::clone() const {
    return new ChooserNameFilter(*this);
  }


  QString ChooserNameFilter::getImageDescription() const {
    QString description = AbstractFilter::getImageDescription();

    if (getMinForSuccess() == 1)
      description += "point with it's chooser name ";
    else
      description += "points with chooser names ";

    description += descriptionSuffix();
    return description;
  }


  QString ChooserNameFilter::getPointDescription() const {
    return "have chooser names " + descriptionSuffix();
  }
}
