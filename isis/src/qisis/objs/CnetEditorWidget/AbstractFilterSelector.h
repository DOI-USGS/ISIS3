#ifndef AbstractFilterSelector_H
#define AbstractFilterSelector_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


// parent
#include <QWidget>

// included because they are needed inside a templated method
#include "AbstractFilter.h"


class QComboBox;
class QHBoxLayout;
class QPushButton;
class QReadWriteLock;

namespace Isis {
  class ControlNet;
  class ControlMeasure;
  class ControlPoint;

  /**
   * @brief Base class for filter selectors
   *
   * Filter selectors are combo boxes with filters as elements. A filter
   * selector has only one active filter at a time. These allow the user to
   * choose which filter to apply to the control net.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2015-11-16 Ian Humphrey - Removed embedded close button icon. References #1041.
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   */
  class AbstractFilterSelector : public QWidget {
      Q_OBJECT

    signals:
      void close(AbstractFilterSelector *);
      void filterChanged();
      void sizeChanged();


    public:
      AbstractFilterSelector();
      virtual ~AbstractFilterSelector();

      template< typename Evaluatable >
      bool evaluate(const Evaluatable *evaluatable) const {
        return m_filter && m_filter->evaluate(evaluatable);
      }

      bool hasFilter() const;
      bool hasFilter(bool (AbstractFilter:: *)() const) const;

      QString getDescription(QString(AbstractFilter:: *)() const) const;

      AbstractFilterSelector &operator=(const AbstractFilterSelector &other);


    public slots:
      void sendClose();


    protected:
      void nullify();
      virtual void createSelector();
      QComboBox *getSelector() const;
      QHBoxLayout *getMainLayout() const;
      AbstractFilter *getFilter() const;
      void setFilter(AbstractFilter *);


    protected slots:
      virtual void changeFilter(int index) = 0;
      virtual void deleteFilter();


      // disable copying of this class which can't exist anyway ....uhhuh...yeah
    private:
      AbstractFilterSelector(const AbstractFilterSelector &other);


    private:
      QComboBox *m_selector;
      QHBoxLayout *m_mainLayout;
      QPushButton *m_closeButton;
      AbstractFilter *m_filter;
  };
}

#endif
