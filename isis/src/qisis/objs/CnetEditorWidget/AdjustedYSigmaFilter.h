#ifndef AdjustedYSigmaFilter_H
#define AdjustedYSigmaFilter_H

#include "AbstractNumberFilter.h"

template< typename U, typename V > struct QPair;
class QString;

namespace Isis {
  class AbstractFilterSelector;
  class ControlMeasure;
  class ControlNet;
  class ControlPoint;

  /**
   * @brief Allows filtering by adjusted surface point Y sigma
   *
   * This class allows the user to filter control points and control measures
   * by adjusted surface point Y sigma. This allows the user to make a
   * list of control points that are less than or greater than a certain
   * adjusted surface point Y sigma.
   *
   * @author 2019-07-26 Ken Edmundson
   *
   * @internal
   *   @history 2019-07-26 Ken Edmundson - Original version based off of the
   *                           AdjustedLongitudeSigmaFilter class.
   */
  class AdjustedYSigmaFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      AdjustedYSigmaFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
      AdjustedYSigmaFilter(const AdjustedYSigmaFilter &other);
      virtual ~AdjustedYSigmaFilter();

      bool evaluate(const QPair<QString, ControlNet *> *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
