#ifndef SampleResidualFilter_H
#define SampleResidualFilter_H

#include "AbstractNumberFilter.h"

template< typename U, typename V > struct QPair;
class QString;

namespace Isis {
  class ControlMeasure;
  class ControlNet;
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
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   *   @history 2018-09-28 Kaitlyn Lee - Changed the declaration of QPair from class to struct.
   *                           Fixes build warning on MacOS 10.13. References #5520.
   */
  class SampleResidualFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      SampleResidualFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
      SampleResidualFilter(const SampleResidualFilter &other);
      virtual ~SampleResidualFilter();

      bool evaluate(const QPair<QString, ControlNet *> *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
      QString getMeasureDescription() const;
  };
}

#endif
