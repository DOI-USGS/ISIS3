#ifndef Tab_H
#define Tab_H

/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2010/05/07 20:39:19 $
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


// QToolButton is the parent of this class, and should be the ONLY include
// in this file.  Do not add any other includes to this header file!
#include <QToolButton>

class QWidget;
class QPaintEvent;

namespace Qisis
{
  /**
   * A Tab is simply a QToolButton which shows or hides some other QWidget,
   * which we call associatedWidget.  The Tab does not own this widget, it
   * just stores a pointer so that it can set the widget to be visible or
   * invisible.  Tabs are toggle buttons which stay down when clicked, and then
   * come back up if clicked again.  If a Tab is down then its associatedWidget
   * is visible.  If a Tab is up then its associatedWidget is invisible.  Tabs
   * are designed to be added to TabBars, which are special QToolBars that can
   * handle the management and storing of Tabs.  Tabs have a "radioGroup" to
   * which they belong, which is just an int.  Tabs in the same TabBar which
   * have the same radioGroup number have the property such that only one of the
   * Tabs can be selected at a time.  Note that the radioGroup value is only
   * used if the TabBar for which the Tab resides has a radioStyle() of true,
   * which is false by default.  See TabBar for more details about radioStyle.
   *
   * @author Eric Hyer
   *
   * @internal
   * @history 2010-05-06 Eric Hyer - Original Version
   */
  class Tab : public QToolButton
  {
      Q_OBJECT
  
    public:
      Tab(QWidget * associatedWidget, QWidget * parent = 0);
      Tab(const Tab & other);
      
      virtual ~Tab();
      
      void setPosition(const int & newPosition);
      const int & getPosition() const;
      void setRadioGroup(const int & newRadioGroup);
      const int & getRadioGroup() const;
      void setSelected(bool newStatus);
      bool isSelected();
      
      
    signals:
      void clicked(const int &);
      
      
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
