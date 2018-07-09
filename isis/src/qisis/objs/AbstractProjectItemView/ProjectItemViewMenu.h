#ifndef ProjectItemViewMenu_h
#define ProjectItemViewMenu_h
/**
 * @file
 * $Date$
 * $Revision$
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <QMenu>

namespace Isis {

  /**
   * QMenu subclass that overrides the closeEvent. Used in views to disable
   * actions when a menu is visible and a user clicks outside of a view.
   *
   * @author 2018-06-27 Kaitlyn Lee
   *
   * @internal
   *   @history 2018-06-27 Kaitlyn Lee - Original version.
   */

  class ProjectItemViewMenu : public QMenu {
    Q_OBJECT

    public:
      ProjectItemViewMenu(const QString &title, QWidget *parent = 0) : QMenu(title, parent){};

    signals:
      void menuClosed();

    private:
      void closeEvent(QCloseEvent *event);
  };
}

#endif
