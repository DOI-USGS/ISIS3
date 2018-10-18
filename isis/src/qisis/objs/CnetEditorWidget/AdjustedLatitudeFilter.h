#ifndef AdjustedLatitudeFilter_H
#define AdjustedLatitudeFilter_H

#include "AbstractNumberFilter.h"

template< typename U, typename V > struct QPair;
class QString;

namespace Isis {
  class AbstractFilterSelector;
  class ControlMeasure;
  class ControlPoint;

  /**
   * @brief Allows filtering by adjusted surface point latitude
   *
   * This class allows the user to filter control points and control measures
   * by adjusted surface point latitude. This allows the user to make a list
   * of control points that are less than or greater than a certain adjusted
   * surface point latitude.
   *
   * @author 2012-04-23 Jai Rideout
   *
   * @internal
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   *   @history 2018-09-28 Kaitlyn Lee - Changed the declaration of QPair from class to struct.
   *                           Fixes build warning on MacOS 10.13. References #5520.
   */
  class AdjustedLatitudeFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      AdjustedLatitudeFilter(AbstractFilter::FilterEffectivenessFlag flag,
                             int minimumForSuccess = -1);
      AdjustedLatitudeFilter(const AdjustedLatitudeFilter &other);
      virtual ~AdjustedLatitudeFilter();

      bool evaluate(const QPair<QString, ControlNet *> *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
