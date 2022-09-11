/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
