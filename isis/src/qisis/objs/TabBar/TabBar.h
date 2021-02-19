#ifndef TabBar_H
#define TabBar_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// QToolBar is the parent of this class, and should be the ONLY include
// in this file.  Do not add any other includes to this header file!
#include <QToolBar>

class QWidget;
template< class A > class QVector;

namespace Isis
{
  class Tab;

  /**
   * A TabBar is a QToolBar which is specifically designed to store and manage
   * Tabs, which are specialized QToolButtons that can hide and show other
   * QWidgets (see Tab).
   *
   * @author 2010-05-07 Eric Hyer
   *
   * @see Tab
   *
   * @internal
   * @history 2010-05-07 Eric Hyer - Original Version
   * @history 2010-06-03 Eric Hyer - Added noneSelected method
   * @history 2010-09-23 Eric Hyer - Tab's parent is now QAction
   *                               - Removed useDefaults var and functionality
   *                               - Fixed namespace issue
   */
  class TabBar : public QToolBar
  {
      Q_OBJECT

    public:
      TabBar();
      virtual ~TabBar();

      virtual void addTab(Tab * newTab);
      int curSelectedTab() const;
      void setRadioStyle(const bool & radioStyle);
      bool radioStyle();
      int size() const;

      void setSelected(const int & index, const bool & status);
      bool isSelected(const int & index);
      bool noneSelected();
      void setEnabled(bool);


    private slots:
      void tabClicked(const int & index);


    private:
      // functions
      bool noOthersInGrpSelected(const int & index) const;
      void deselectOthersInGrp(const int & index);


    private:
      // data
      QVector< Tab * > * tabs;
      bool radioStyleTabs;
  };
}

#endif
