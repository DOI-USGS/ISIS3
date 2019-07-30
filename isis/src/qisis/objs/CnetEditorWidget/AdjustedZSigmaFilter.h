#ifndef AdjustedZSigmaFilter_H
#define AdjustedZSigmaFilter_H

#include "AbstractNumberFilter.h"

template< typename U, typename V > struct QPair;
class QString;

namespace Isis {
  class ControlMeasure;
  class ControlNet;
  class ControlPoint;

  /**
   * @brief Allows filtering by adjusted surface point Z sigma
   *
   * This class allows the user to filter control points and control measures
   * by adjusted surface point Z sigma. This allows the user to make a
   * list of control points that are less than or greater than a certain
   * adjusted surface point Z sigma.
   *
   * @author 2019-07-26 Ken Edmundson
   *
   * @internal
   *   @history 2019-07-26 Ken Edmundson - Original version based off of the
   *                           AdjustedRadiusSigmaFilter class.
   */
  class AdjustedZSigmaFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      AdjustedZSigmaFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
      AdjustedZSigmaFilter(const AdjustedZSigmaFilter &other);
      virtual ~AdjustedZSigmaFilter();

      bool evaluate(const QPair<QString, ControlNet *> *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
