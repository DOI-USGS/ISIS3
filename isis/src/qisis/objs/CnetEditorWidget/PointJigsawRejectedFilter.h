#ifndef PointJigsawRejectedFilter_H
#define PointJigsawRejectedFilter_H

#include "AbstractFilter.h"


namespace Isis {
  class ControlCubeGraphNode;
  class ControlPoint;
  class ControlMeasure;

  /**
   * @brief Allows filtering by a control point's jigsaw rejected status
   *
   * This class allows the user to filter control points based on whether or
   * not they are rejected by jigsaw. This allows the user to make a list of
   * jigsaw rejected or not-jigsaw rejected control points.
   *
   * @author 2012-04-18 Jai Rideout
   *
   * @internal 
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054. 
   */
  class PointJigsawRejectedFilter : public AbstractFilter {
      Q_OBJECT

    public:
      PointJigsawRejectedFilter(AbstractFilter::FilterEffectivenessFlag flag,
          int minimumForSuccess = -1);
      PointJigsawRejectedFilter(const AbstractFilter &other);
      virtual ~PointJigsawRejectedFilter();

      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif

