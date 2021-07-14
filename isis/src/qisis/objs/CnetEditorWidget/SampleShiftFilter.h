#ifndef SampleShiftFilter_H
#define SampleShiftFilter_H

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
   * @brief Allows filtering by a control measure's sample shift
   *
   * This class allows the user to filter control measures by their sample
   * shift (i.e. how many samples they shifted in the image). This allows the
   * user to make a list of control measures that shifted by a certain amount
   * in an image after adjustment. The sample shift is the difference between
   * the measure's sample and a priori sample.
   *
   * @author 2012-04-18 Jai Rideout
   *
   * @internal
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   *   @history 2018-09-28 Kaitlyn Lee - Changed the declaration of QPair from class to struct.
   *                           Fixes build warning on MacOS 10.13. References #5520.
   */
  class SampleShiftFilter : public AbstractNumberFilter {
      Q_OBJECT

    public:
      SampleShiftFilter(AbstractFilter::FilterEffectivenessFlag flag,
            int minimumForSuccess = -1);
      SampleShiftFilter(const SampleShiftFilter &other);
      virtual ~SampleShiftFilter();

      bool evaluate(const QPair<QString, ControlNet *> *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
      QString getMeasureDescription() const;
  };
}

#endif
