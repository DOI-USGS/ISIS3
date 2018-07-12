#ifndef LineFilter_H
#define LineFilter_H

#include "AbstractNumberFilter.h"

template< typename U, typename V > class QPair;
class QString;

namespace Isis {
  class ControlMeasure;
  class ControlNet;
  class ControlPoint;

  /**
   * @brief Allows filtering by a control measure's line
   *
   * This class allows the user to filter control measures by their line (i.e.
   * which line they are positioned at in the image). This allows the user to
   * make a list of control measures that are too close to the edge of an
   * image after pointreg adjustment.
   *
   * @author 2012-01-05 Jai Rideout
   *
   * @internal
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   */
  class LineFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      LineFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
      LineFilter(const LineFilter &other);
      virtual ~LineFilter();

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
