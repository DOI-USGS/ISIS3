#ifndef PointIdFilter_H
#define PointIdFilter_H

#include "AbstractStringFilter.h"


class QString;


namespace Isis {
  class AbstractFilterSelector;
  class ControlPoint;
  class ControlMeasure;

  /**
   * @brief Filter by control point id string
   *
   * This class allows the user to filter control points based on what the
   * control point id is. This allows the user to find a particular control
   * point or make a list of control points with similar ids.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   */
  class PointIdFilter : public AbstractStringFilter {
      Q_OBJECT

    public:
      PointIdFilter(AbstractFilter::FilterEffectivenessFlag,
          ControlNet *network, int minimumForSuccess = -1);
      PointIdFilter(const PointIdFilter &other);
      virtual ~PointIdFilter();

      bool evaluate(const QString *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
