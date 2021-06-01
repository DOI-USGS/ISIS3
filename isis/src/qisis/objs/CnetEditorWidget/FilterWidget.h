#ifndef FilterWidget_H
#define FilterWidget_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// parent
#include <QWidget>

// these are included because it is needed inside a templated method
#include "FilterGroup.h"


class QLabel;
template< class T > class QList;
template< typename U, typename V > struct QPair;
class QPushButton;
class QString;
class QTextEdit;
class QVBoxLayout;


namespace Isis {
  class ControlNet;
  class ControlMeasure;
  class ControlPoint;
  class FilterGroup;

  /**
   * This widget contains filtering capabilities for a single filter type.
   *
   * The filter types are (strings containing exactly):
   *   Connections
   *   Images and Points
   *   Points and Measures
   *
   * @todo Add type safety to filter types.
   *
   * This class does the visualization and editing of the filter groups. An
   * interface is provided for evaluating all of the contained filters.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2015-11-16 Ian Humphrey - Removed embedded icons. References #1041.
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   *   @history 2018-09-28 Kaitlyn Lee - Changed the declaration of QPair from class to struct.
   *                           Fixes build warning on MacOS 10.13. References #5520.
   */
  class FilterWidget : public QWidget {
      Q_OBJECT

    public:
      explicit FilterWidget(QString);
      FilterWidget(const FilterWidget &);
      virtual ~FilterWidget();

      template< typename T >
      bool evaluate(const T *t, bool (AbstractFilter::*meth)() const) const {
        // if andFiltersTogether is true then we break out of the loop as
        // soon as any selectors evaluate to false.  If andFiltersTogether
        // is false then we are ORing them so we break out as soon as any
        // selector evaluates to true.  Whether we are looking for
        // successes or failures depends on whether we are ANDing or ORing
        // the filters (selectors) together!!!
        bool looking = true;
        for (int i = 0; looking && i < m_filterGroups->size(); i++) {
          if (m_filterGroups->at(i)->hasFilter(meth))
            looking = !(m_filterGroups->at(i)->evaluate(t, meth) ^
                      m_andGroupsTogether);
        }

        // It is good that we are still looking for failures if we were
        // ANDing filters together, but it is bad if we were ORing them
        // since in this case we were looking for success (unless of
        // course there were no filters to look through).
        return !(looking ^ m_andGroupsTogether) || !hasFilter(meth);
      }

      bool evaluate(const QPair<QString, ControlNet *> *) const;
      bool evaluate(const ControlPoint *point) const;
      bool evaluate(const ControlMeasure *measure) const;

      bool hasFilter(bool (AbstractFilter:: *)() const) const;

      FilterWidget &operator=(FilterWidget other);


    signals:
      void filterChanged();
      void scrollToBottom();


    private:
      void nullify();
      void init();
      QList< FilterGroup * > groupsWithCondition(
        bool (FilterGroup:: *)() const) const;

      void updateDescription(QLabel *label,
          bool (AbstractFilter:: *)() const,
          QString(AbstractFilter:: *)() const,
          QString);

      void addGroup(FilterGroup *newGroup);


    private slots:
      void addGroup();
      void deleteGroup(FilterGroup *);
      void changeGroupCombinationLogic(int);
      void updateDescription();
      void maybeScroll(FilterGroup *);


    private:
      QPushButton *m_addGroupButton;
      QButtonGroup *m_buttonGroup;
      QLabel *m_imageDescription;
      QLabel *m_imageDummy;
      QLabel *m_pointDescription;
      QLabel *m_pointDummy;
      QLabel *m_measureDescription;
      QLabel *m_measureDummy;
      QVBoxLayout *m_mainLayout;
      QWidget *m_logicWidget;

      bool m_andGroupsTogether;

      QList< FilterGroup * > * m_filterGroups;
      QString *m_filterType;
  };
}

#endif
