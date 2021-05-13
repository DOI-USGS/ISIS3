#ifndef AbstractNumberFilter_H
#define AbstractNumberFilter_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// parent
#include "AbstractFilter.h"


class QButtonGroup;
class QLineEdit;
class QString;


namespace Isis {
  class ControlMeasure;
  class ControlPoint;

  /**
   * @brief Base class for filters that are number-based
   *
   * This class is the base class that all filters that are number-based.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   */
  class AbstractNumberFilter : public AbstractFilter {
      Q_OBJECT

    public:
      AbstractNumberFilter(AbstractFilter::FilterEffectivenessFlag,
                           int minimumForSuccess = -1);
      AbstractNumberFilter(const AbstractNumberFilter &other);
      virtual ~AbstractNumberFilter();


    protected:
      using Isis::AbstractFilter::evaluate;
      bool evaluate(double) const;
      QString descriptionSuffix() const;
      bool lessThan() const;


    private:
      void createWidget();
      void nullify();


    private slots:
      void updateLineEditText(QString);


    private:
      QButtonGroup *m_greaterThanLessThan;
      QLineEdit *m_lineEdit;
      QString *m_lineEditText;
  };
}

#endif
