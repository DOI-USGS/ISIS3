#ifndef AdjustedXSigmaFilter_H
#define AdjustedXSigmaFilter_H

#include "AbstractNumberFilter.h"

template< typename U, typename V > struct QPair;
class QString;

namespace Isis {
  class AbstractFilterSelector;
  class ControlMeasure;
  class ControlNet;
  class ControlPoint;

  /**
   * @brief Allows filtering by adjusted surface point X sigma
   *
   * This class allows the user to filter control points and control measures
   * by adjusted surface point X sigma. This allows the user to make a
   * list of control points that are less than or greater than a certain
   * adjusted surface point X sigma.
   *
   * @author 2019-07-26 Ken Edmundson
   *
   * @internal
   *   @history 2019-07-26 Ken Edmundson - Original version based off of the
   *                           AdjustedLatitudeSigmaFilter class.
   */
  class AdjustedXSigmaFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      AdjustedXSigmaFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
      AdjustedXSigmaFilter(const AdjustedXSigmaFilter &other);
      virtual ~AdjustedXSigmaFilter();

      bool evaluate(const QPair<QString, ControlNet *> *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
