#ifndef SampleShiftFilter_H
#define SampleShiftFilter_H

#include "AbstractNumberFilter.h"


class QString;


namespace Isis {
  class AbstractFilterSelector;
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;

  /**
   * @brief Allows filtering by a control measure's sample shift
   *
   * This class allows the user to filter control measures by their sample
   * shift (i.e. how many samples they shifted in the image). This allows the
   * user to make a list of control measures that shifted by a certain amount
   * in an image after adjustment. The sample shift is the difference between
   * the measure's sample and a priori sample.
   *
   * @author 2012-04-18 Jai Rideout
   *
   * @internal 
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054. 
   */
  class SampleShiftFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      SampleShiftFilter(AbstractFilter::FilterEffectivenessFlag flag,
          int minimumForSuccess = -1);
      SampleShiftFilter(const SampleShiftFilter &other);
      virtual ~SampleShiftFilter();

      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
      QString getMeasureDescription() const;
  };
}

#endif

