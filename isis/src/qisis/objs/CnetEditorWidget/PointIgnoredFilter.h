#ifndef PointIgnoredFilter_H
#define PointIgnoredFilter_H

#include "AbstractFilter.h"

template< typename U, typename V > struct QPair;
class QString;

namespace Isis {
  class ControlMeasure;
  class ControlNet;
  class ControlPoint;

  /**
   * @brief Allows filtering by a control point's ignored status
   *
   * This class allows the user to filter control points based on whether or
   * not they are ignored. This allows the user to make a list of ignored or
   * not-ignored control points.
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
  class PointIgnoredFilter : public AbstractFilter {
      Q_OBJECT

    public:
      PointIgnoredFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
      PointIgnoredFilter(const AbstractFilter &other);
      virtual ~PointIgnoredFilter();

      bool evaluate(const QPair<QString, ControlNet *> *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
