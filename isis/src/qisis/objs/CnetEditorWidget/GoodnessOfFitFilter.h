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
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   */
  class GoodnessOfFitFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      GoodnessOfFitFilter(AbstractFilter::FilterEffectivenessFlag flag,
          ControlNet *network, int minimumForSuccess = -1);
      GoodnessOfFitFilter(const GoodnessOfFitFilter &other);
      virtual ~GoodnessOfFitFilter();

      bool evaluate(const QString *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
      QString getMeasureDescription() const;
  };
}

#endif
