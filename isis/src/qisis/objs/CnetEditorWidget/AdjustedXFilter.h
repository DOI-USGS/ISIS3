#ifndef AdjustedXFilter_H
#define AdjustedXFilter_H

#include "AbstractNumberFilter.h"

template< typename U, typename V > struct QPair;
class QString;

namespace Isis {
  class AbstractFilterSelector;
  class ControlMeasure;
  class ControlPoint;

  /**
   * @brief Allows filtering by adjusted surface point X
   *
   * This class allows the user to filter control points and control measures
   * by adjusted surface point X. This allows the user to make a list
   * of control points that are less than or greater than a certain adjusted
   * surface point X.
   *
   * @author 2019-07-26 Ken Edmundson
   *
   * @internal
   *   @history 2019-07-26 Ken Edmundson - Original version based off of the AdjustedLatitudeFilter
   *                           class.
   */
  class AdjustedXFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      AdjustedXFilter(AbstractFilter::FilterEffectivenessFlag flag,
                             int minimumForSuccess = -1);
      AdjustedXFilter(const AdjustedXFilter &other);
      virtual ~AdjustedXFilter();

      bool evaluate(const QPair<QString, ControlNet *> *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
