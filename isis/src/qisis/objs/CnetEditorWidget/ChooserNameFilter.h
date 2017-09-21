#ifndef ChooserNameFilter_H
#define ChooserNameFilter_H

#include "AbstractStringFilter.h"


class QString;


namespace Isis {
  class AbstractFilterSelector;
  class ControlPoint;
  class ControlMeasure;

  /**
   * @brief Allows filtering by the chooser name
   *
   * This class allows the user to filter control points by chooser name. This
   * allows the user to make a list of control points which have been chosen
   * by a particular user or application.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal 
   *    @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class ChooserNameFilter : public AbstractStringFilter {
      Q_OBJECT

    public:
      ChooserNameFilter(AbstractFilter::FilterEffectivenessFlag,
          int minimumForSuccess = -1);
      ChooserNameFilter(const ChooserNameFilter &other);
      virtual ~ChooserNameFilter();

      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
