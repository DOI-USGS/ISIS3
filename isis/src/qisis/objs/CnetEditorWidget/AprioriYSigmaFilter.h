#ifndef APrioriYSigmaFilter_H
#define APrioriYSigmaFilter_H

#include "AbstractNumberFilter.h"

template< typename U, typename V > struct QPair;
class QString;

namespace Isis {
  class ControlMeasure;
  class ControlNet;
  class ControlPoint;

  /**
   * @brief Allows filtering by a priori surface point Y sigma
   *
   * This class allows the user to filter control points and control measures
   * by a priori surface point Y sigma. This allows the user to make a
   * list of control points that are less than or greater than a certain
   * a priori surface point Y sigma.
   *
   * @author 2019-07-26 Ken Edmundson
   *
   * @internal
   *   @history 2019-07-26 Ken Edmundson - Original version based off of the
   *                           APrioriLongitudeSigmaFilter class.
   *   @history 2019-11-22 Ken Edmundson - Minor text corrections to returned QString in
   *                           getImageDescription() and getPointDescription() methods.
   */
  class APrioriYSigmaFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      APrioriYSigmaFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
      APrioriYSigmaFilter(const APrioriYSigmaFilter &other);
      virtual ~APrioriYSigmaFilter();

      bool evaluate(const QPair<QString, ControlNet *> *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
