#ifndef MeasureIgnoredFilter_H
#define MeasureIgnoredFilter_H

#include "AbstractFilter.h"


namespace Isis {
  class AbstractFilterSelector;
  class ControlCubeGraphNode;
  class ControlMeasure;

  /**
   * @brief Allows filtering by a control measure's ignored status
   *
   * This class allows the user to filter control measures based on whether or
   * not they are ignored. This allows the user to make a list of ignored or
   * not-ignored control measures.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *    @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   */
  class MeasureIgnoredFilter : public AbstractFilter {
      Q_OBJECT

    public:
      MeasureIgnoredFilter(AbstractFilter::FilterEffectivenessFlag flag,
          ControlNet *network, int minimumForSuccess = -1);
      virtual ~MeasureIgnoredFilter();

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
