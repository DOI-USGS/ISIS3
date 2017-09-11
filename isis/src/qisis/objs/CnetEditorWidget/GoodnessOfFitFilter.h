#ifndef GoodnessOfFitFilter_H
#define GoodnessOfFitFilter_H

#include "AbstractNumberFilter.h"


class QString;


namespace Isis {
  class AbstractFilterSelector;
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;

  /**
   * @brief Allows filtering by goodness of fit
   *
   * This class allows the user to filter control points and control measures
   * by goodness of fit. This allows the user to make a list of control
   * points that are potentially mis-registered.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal 
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054. 
   *    
   */
  class GoodnessOfFitFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      GoodnessOfFitFilter(AbstractFilter::FilterEffectivenessFlag flag,
          int minimumForSuccess = -1);
      GoodnessOfFitFilter(const GoodnessOfFitFilter &other);
      virtual ~GoodnessOfFitFilter();

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
