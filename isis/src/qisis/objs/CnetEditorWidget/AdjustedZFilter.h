#ifndef AdjustedZFilter_H
#define AdjustedZFilter_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractNumberFilter.h"

template< typename U, typename V > struct QPair;
class QString;

namespace Isis {
  class ControlMeasure;
  class ControlNet;
  class ControlPoint;

  /**
   * @brief Allows filtering by adjusted surface point Z
   *
   * This class allows the user to filter control points and control measures
   * by adjusted surface point Z. This allows the user to make a list
   * of control points that are less than or greater than a certain adjusted
   * surface point Z.
   *
   * @author 2019-07-26 Ken Edmundson
   *
   * @internal
   *   @history 2019-07-26 Ken Edmundson - Original version based off of the AdjustedRadiusFilter
   *                           class.
   *   @history 2019-11-22 Ken Edmundson - Minor text corrections to returned QString in
   *                           getImageDescription() and getPointDescription() methods.
   */
  class AdjustedZFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      AdjustedZFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
      AdjustedZFilter(const AdjustedZFilter &other);
      virtual ~AdjustedZFilter();

      bool evaluate(const QPair<QString, ControlNet *> *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
