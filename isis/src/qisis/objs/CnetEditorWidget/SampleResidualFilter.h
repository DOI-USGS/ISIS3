#ifndef SampleResidualFilter_H
#define SampleResidualFilter_H

#include "AbstractNumberFilter.h"


class QString;


namespace Isis {
  class AbstractFilterSelector;
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;

  /**
   * @brief Allows filtering by the sample residual
   *
   * This class allows the user to filter control measures by how much the
   * sample coordinate moved. This allows the user to make a list of control
   * measures which have been significantly adjusted by pointreg.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal 
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054. 
   */
  class SampleResidualFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      SampleResidualFilter(AbstractFilter::FilterEffectivenessFlag flag,
          int minimumForSuccess = -1);
      SampleResidualFilter(const SampleResidualFilter &other);
      virtual ~SampleResidualFilter();

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
