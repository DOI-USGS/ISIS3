#ifndef MeasureCountFilter_H
#define MeasureCountFilter_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractFilter.h"

class QButtonGroup;
class QLineEdit;
template< typename U, typename V > struct QPair;
class QSpinBox;
class QString;


namespace Isis {
  class ControlMeasure;
  class ControlNet;
  class ControlPoint;

  /**
   * @brief Allows filtering by the number of measures in a control point
   *
   * This class allows the user to filter control points by the number of
   * measures they contain. This allows the user to make a list of control
   * points which are shallow or deep.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   *   @history 2018-09-28 Kaitlyn Lee - Changed the declaration of QPair from class to struct.
   *                           Fixes build warning on MacOS 10.13. References #5520.
   */
  class MeasureCountFilter : public AbstractFilter {
      Q_OBJECT

    public:
      MeasureCountFilter(AbstractFilter::FilterEffectivenessFlag,
            int minimumForSuccess = -1);
      MeasureCountFilter(const MeasureCountFilter &other);
      virtual ~MeasureCountFilter();

      bool evaluate(const QPair<QString, ControlNet *> *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;


    private:
      void createWidget();
      void init();


    private slots:
      void updateMinMax(int);
      void updateMeasureCount(int);


    private:
      QButtonGroup *m_minMaxGroup;
      QSpinBox *m_countSpinBox;
      int m_count;
      bool m_minimum;
  };
}

#endif
