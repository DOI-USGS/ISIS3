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
#include "MenuItem.h"

namespace Isis {

  /**
   * Constructs a MenuItem with the QMenu constructor
   *
   * @param title  Title of the menu
   *
   * @param parent Parent widget
   */
  //  MenuItem::MenuItem(const QString &title, QWidget *parent = 0) {
  //   QMenu(title, parent);
  // }


  /**
   * Overrides the closeEvent to emit the signal menuClosed().
   * menuClosed() is connected to the slot disableActions() in a view.
   *
   * @param event The close event
   */
  void MenuItem::closeEvent(QCloseEvent *event) {
    emit menuClosed();
  }
}
