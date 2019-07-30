#ifndef AdjustedYFilter_H
#define AdjustedYFilter_H

#include "AbstractNumberFilter.h"

template< typename U, typename V > struct QPair;
class QString;

namespace Isis {
  class AbstractFilterSelector;
  class ControlMeasure;
  class ControlPoint;
  class ControlNet;

  /**
   * @brief Allows filtering by adjusted surface point Y
   *
   * This class allows the user to filter control points and control measures
   * by adjusted surface point Y. This allows the user to make a list
   * of control points that are less than or greater than a certain adjusted
   * surface point Y.
   *
   * @author 2019-07-26 Ken Edmundson
   *
   * @internal
   *   @history 2019-07-26 Ken Edmundson - Original version based off of the
   *                           AdjustedLongitudeFilter class.
   */
  class AdjustedYFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      AdjustedYFilter(AbstractFilter::FilterEffectivenessFlag flag,
                              int minimumForSuccess = -1);
      AdjustedYFilter(const AdjustedYFilter &other);
      virtual ~AdjustedYFilter();

      bool evaluate(const QPair<QString, ControlNet *> *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
