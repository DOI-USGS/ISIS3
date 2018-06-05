#ifndef ResidualMagnitudeFilter_H
#define ResidualMagnitudeFilter_H

#include "AbstractNumberFilter.h"


class QString;


namespace Isis {
  class AbstractFilterSelector;
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;

  /**
   * @brief Filters by residual magnitude
   *
   * This class handles filtering by residual magnitudes.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   */
  class ResidualMagnitudeFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      ResidualMagnitudeFilter(AbstractFilter::FilterEffectivenessFlag flag,
          ControlNet *network, int minimumForSuccess = -1);
      ResidualMagnitudeFilter(const ResidualMagnitudeFilter &other);
      virtual ~ResidualMagnitudeFilter();

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
