#ifndef AprioriXFilter_H
#define AprioriXFilter_H

#include "AbstractNumberFilter.h"

template< typename U, typename V > struct QPair;
class QString;

namespace Isis {
  class ControlMeasure;
  class ControlNet;
  class ControlPoint;

  /**
   * @brief Allows filtering by a priori surface point X
   *
   * This class allows the user to filter control points and control measures
   * by a priori surface point X. This allows the user to make a list
   * of control points that are less than or greater than a certain a priori
   * surface point X.
   *
   * @author 2019-07-26 Ken Edmundson
   *
   * @internal
   *   @history 2019-07-26 Ken Edmundson - Original version based off of the
   *                           APrioriLatitudeFilter class.
   */
  class AprioriXFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      AprioriXFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
      AprioriXFilter(const AprioriXFilter &other);
      virtual ~AprioriXFilter();

      bool evaluate(const QPair<QString, ControlNet *> *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
