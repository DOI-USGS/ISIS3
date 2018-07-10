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
#include "IsisDebug.h"

#include "ControlPointEditView.h"

#include <QAction>
#include <QList>
#include <QSize>
#include <QSizePolicy>
#include <QToolBar>
#include <QWidgetAction>

#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlPointEditWidget.h"
#include "Directory.h"
#include "ToolPad.h"


namespace Isis {
  /**
   * Constructor.
   */
  ControlPointEditView::ControlPointEditView(Directory *directory, QWidget *parent) :
                        AbstractProjectItemView(parent) {

    m_controlPointEditWidget = new ControlPointEditWidget(directory, parent);

    //  TODO How are control nets and serial number lists going to be handled?  I assume there can
    //       be several active control nets depending on the view, ie. jigsaw might operate on one
    //       net, while the editors might be using a different net.  Will Directory keep track?
    //

    setCentralWidget(m_controlPointEditWidget);

    // Store the buttons (actions) for easy enable/disable.
    m_buttons = m_controlPointEditWidget->findChildren<QPushButton *>();

    // On default, actions are disabled until the cursor enters the view.
    disableActions();
  }


  /**
   * Destructor
   */
  ControlPointEditView::~ControlPointEditView() {
    delete m_controlPointEditWidget;
  }


  /**
   * Returns the ControlPointEditWidget.
   *
   * @return (ControlPointEditWidget *) The ControlPointEditWidget used to
   *         display the footprints.
   */
  ControlPointEditWidget *ControlPointEditView::controlPointEditWidget() {
    return m_controlPointEditWidget;
  }


  /**
   * Disables buttons/actions. Overriden method.
   */
  void ControlPointEditView::disableActions() {
    foreach (QPushButton *button, m_buttons) {
      button->setDisabled(true);
    }
  }


  /**
   * Enables buttons/actions. Overriden method.
   */
  void ControlPointEditView::enableActions() {
    foreach (QPushButton *button, m_buttons) {
      button->setEnabled(true);
    }
  }
}
