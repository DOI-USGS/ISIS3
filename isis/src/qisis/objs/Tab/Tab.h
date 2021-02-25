#ifndef Tab_H
#define Tab_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// QAction is the parent of this class, and should be the ONLY include
// in this file.  Do not add any other includes to this header file!
#include <QAction>

class QWidget;

namespace Isis
{
  /**
   * A Tab is a QAction which shows or hides some other QWidget, which we call
   * associatedWidget.  The Tab does not own this widget, it just stores a
   * pointer so that it can set the widget to be visible or invisible.  Tabs
   * are toggleable.  If a Tab is down then its associatedWidget is visible
   * If a Tab is up then its associatedWidget is invisible.  Tabs are designed
   * to be added to TabBars, which are special QToolBars that can handle the
   * management and storing of Tabs.  Tabs have a "radioGroup" to which they
   * belong, which is just an int.  Tabs in the same TabBar which have the same
   * radioGroup number have the property such that only one of the Tabs can be
   * selected at a time.  Note that the radioGroup value is only used if the
   * TabBar for which the Tab resides has a radioStyle() of true, which is
   * false by default.  See TabBar for more details about radioStyle.
   *
   * @author 2010-05-06 Eric Hyer
   *
   * @internal
   * @history 2010-05-06 Eric Hyer - Original Version
   * @history 2010-09-23 Eric Hyer - Parent class now QAction, not QToolButton
   *                               - Fixed namespace issue
   */
  class Tab : public QAction
  {
      Q_OBJECT

    public:
      Tab(QWidget * associatedWidget, QWidget * parent = 0);

      virtual ~Tab();

      void setPosition(const int & newPosition);
      const int & getPosition() const;
      void setRadioGroup(const int & newRadioGroup);
      const int & getRadioGroup() const;
      void setSelected(bool newStatus);
      bool isSelected();

    signals:
      void clicked(const int &);


    private:
      Tab(const Tab & other);
      const Tab & operator=(Tab);


    private slots:
      void handleTriggered();

    private:
      QWidget * associatedWidget;
      int position;
      int radioGroup;
      int location;
      bool selectedStatus;
  };
}

#endif
