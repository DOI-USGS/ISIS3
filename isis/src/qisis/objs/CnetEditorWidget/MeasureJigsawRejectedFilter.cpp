#include "IsisDebug.h"

#include "MeasureJigsawRejectedFilter.h"

#include <iostream>

#include <QHBoxLayout>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"


using std::cerr;


namespace Isis {
  namespace CnetViz {
    MeasureJigsawRejectedFilter::MeasureJigsawRejectedFilter(
      AbstractFilter::FilterEffectivenessFlag flag, int minimumForSuccess) :
      AbstractFilter(flag, minimumForSuccess) {
    }


    MeasureJigsawRejectedFilter::~MeasureJigsawRejectedFilter() {
    }


    bool MeasureJigsawRejectedFilter::evaluate(const ControlCubeGraphNode *node) const {
      return AbstractFilter::evaluateImageFromMeasureFilter(node);
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
      if (getMinForSuccess() == 1)
        description += "measure that is ";
      else
        description += "measures that are ";

      if (inclusive())
        description += "jigsaw rejected";
      else
        description += "not jigsaw rejected";

      return description;
    }


    QString MeasureJigsawRejectedFilter::getPointDescription() const {
      return getImageDescription();
    }


    QString MeasureJigsawRejectedFilter::getMeasureDescription() const {
      QString description = "are ";

      if (inclusive())
        description += "jigsaw rejected";
      else
        description += "not jigsaw rejected";

      return description;
    }
  }
}

