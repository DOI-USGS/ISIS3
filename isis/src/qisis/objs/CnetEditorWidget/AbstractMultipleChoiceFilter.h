#ifndef AbstractMultipleChoiceFilter_H
#define AbstractMultipleChoiceFilter_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// parent
#include "AbstractFilter.h"


class QComboBox;
class QString;


namespace Isis {
  class AbstractFilterSelector;

  /**
   * @brief Base class for combo box type filters
   *
   * This class is the base class that all filters that use combo boxes.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   */
  class AbstractMultipleChoiceFilter : public AbstractFilter {
      Q_OBJECT

    public:
      AbstractMultipleChoiceFilter(AbstractFilter::FilterEffectivenessFlag,
                                   int minimumForSuccess = -1);
      AbstractMultipleChoiceFilter(const AbstractMultipleChoiceFilter &other);
      virtual ~AbstractMultipleChoiceFilter();


    protected:
      void createWidget(QStringList options);
      QString const &getCurrentChoice() const;
      using Isis::AbstractFilter::evaluate;

    private:
      void nullify();


    private slots:
      void updateCurChoice(QString);


    private:
      QComboBox *m_combo;
      QString *m_curChoice;
  };
}

#endif
