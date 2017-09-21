/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
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
#include "ViewControlNet3DWorkOrder.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QtDebug>

#include "Control.h"
#include "ControlList.h"
#include "Directory.h"
#include "Project.h"

namespace Isis {

/**
   * @brief Creates a WorkOrder that will display a control net.
   * @param project The Project that this work order should be interacting with.
   */
  ViewControlNet3DWorkOrder::ViewControlNet3DWorkOrder(Project *project) :
      WorkOrder(project) {
    QAction::setText(tr("&View ControlNet 3D..."));
    m_isSavedToHistory = false;
}


  /**
   * @brief Copies the 'other' work order instance into this new instance.
   * @param other The work order being copied.
   */
  ViewControlNet3DWorkOrder::ViewControlNet3DWorkOrder(const ViewControlNet3DWorkOrder &other) :
      WorkOrder(other) {
  }


  /**
   * @brief The Destructor
   */
  ViewControlNet3DWorkOrder::~ViewControlNet3DWorkOrder() {
  }


  /**
   * @brief Returns a copy of this ViewControlNet3DWorkOrder instance.
   * @return @b (ViewControlNet3DWorkOrder *) A pointer to a copy of this WorkOrder.
   */
  ViewControlNet3DWorkOrder *ViewControlNet3DWorkOrder::clone() const {
    return new ViewControlNet3DWorkOrder(*this);
  }


  /**
   * @brief Determines if there is a control net to display.
   * @param controls  A list of all the control networks in the Project.
   * @return @b bool Returns True if there is exactly one control network to display,
   * False otherwise.
   */
  //tjw 3956
  bool ViewControlNet3DWorkOrder::isExecutable(ControlList *controls) {
    return (controls->count() == 1);
  }


  /**
   * @brief Prompt user for any information need to display the control network.
   * @return @b bool True upon success, False otherwise.
   */
  bool ViewControlNet3DWorkOrder::setupExecution() {
    bool success = WorkOrder::setupExecution();

    if (success) {

//TODO::Find out why this is commented out.

//        JigsawDialog* bundledlg = new JigsawDialog(project());
//        bundledlg->setAttribute(Qt::WA_DeleteOnClose);
//        bundledlg->show();
//      QUndoCommand::setText(tr("&Bundle Adjustment")
//          .arg(controlList().first()->displayProperties()->displayName()));
    }

    return success;
  }


  /**
   * @brief Determines whether a WorkOrder depends upon ViewControlNet3DWorkOrder.
   * @param other  The WorkOrder being checked for dependency.
   * @return @b bool True if their is a dependency, False otherwise.
   */
  bool ViewControlNet3DWorkOrder::dependsOn(WorkOrder *other) const {
    // depend on types of ourselves.
    return dynamic_cast<ViewControlNet3DWorkOrder *>(other);
  }


  /**
   * @brief Display the 3D control network.  Currently not implemented.
   */
  void ViewControlNet3DWorkOrder::execute() {

    //project()->directory()->addCnetEditorView(controlList().first());
    //project()->setClean(false);
  }


  /**
   * @brief Deletes the last view.  Currently not implemented.
   */
  void ViewControlNet3DWorkOrder::undoExecution() {
    //delete project()->directory()->cnetEditorViews().last();
  }
}
