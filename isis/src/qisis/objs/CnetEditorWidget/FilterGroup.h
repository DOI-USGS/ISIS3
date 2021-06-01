#ifndef FilterGroup_H
#define FilterGroup_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// parent
#include <QWidget>

// included because it is needed inside a templated method
#include "AbstractFilterSelector.h"


template< class T > class QList;
class QPushButton;
class QVBoxLayout;


namespace Isis {
  class AbstractFilterSelector;
  class ControlPoint;

  /**
   * This class provides an interface for a group of filters. Filters within
   * a group are logically connected (AND/OR operations). An interface is
   * provided for evaluating all of the filters in the group.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2015-11-16 Ian Humphrey - Removed embedded icons. References #1041.
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class FilterGroup : public QWidget {
      Q_OBJECT

    public:
      explicit FilterGroup(QString type);
      FilterGroup(const FilterGroup &other);
      virtual ~FilterGroup();

      template< typename T >
      bool evaluate(const T *t, bool (AbstractFilter::*meth)() const) const {
        // if andFiltersTogether is true then we break out of the loop as soon
        // as any selectors evaluate to false.  If andFiltersTogether is false
        // then we are ORing them so we break out as soon as any selector
        // evaluates to true.  Whether we are looking for successes or failures
        // depends on whether we are ANDing or ORing the filters (selectors)
        // together!
        bool looking = true;
        for (int i = 0; looking && i < m_selectors->size(); i++)
          if (m_selectors->at(i)->hasFilter(meth))
            looking = !(m_selectors->at(i)->evaluate(t) ^ m_andFiltersTogether);

        // It is good that we are still looking for failures if we were ANDing
        // filters together, but it is bad if we were ORing them since in this
        // case we were looking for success.
        return !(looking ^ m_andFiltersTogether) || !hasFilter(meth);
      }

      //bool hasFilter() const;
      bool hasFilter(bool (AbstractFilter:: *)() const = NULL) const;

      QString getDescription(bool (AbstractFilter:: *)() const,
          QString(AbstractFilter:: *)() const) const;

      bool filtersAreAndedTogether() const;

      FilterGroup &operator=(FilterGroup other);


    signals:
      void close(FilterGroup *);
      void filterChanged();
      void sizeChanged(FilterGroup *);


    private:
      bool hasSelectorWithCondition(
        bool (AbstractFilterSelector:: *)() const) const;
      void nullify();
      void init();
      void addSelector(AbstractFilterSelector *newSelector);


    private slots:
      void addSelector();
      void deleteSelector(AbstractFilterSelector *);
      void sendClose();
      void sendSizeChanged();
      void changeFilterCombinationLogic(int);


    private: // widgets
      QButtonGroup *m_buttonGroup;
      QPushButton *m_closeButton;
      QPushButton *m_newSelectorButton;
      QVBoxLayout *m_groupBoxLayout;
      QWidget *m_logicWidget;


    private:
      QList< AbstractFilterSelector * > * m_selectors;
      bool m_andFiltersTogether;
      QString *m_filterType;
  };
}

#endif
