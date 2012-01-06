#ifndef TabBar_H
#define TabBar_H

/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2010/06/04 03:20:32 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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
      const int curSelectedTab() const;
      void setRadioStyle(const bool & radioStyle);
      bool radioStyle();
      const int size() const;

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
