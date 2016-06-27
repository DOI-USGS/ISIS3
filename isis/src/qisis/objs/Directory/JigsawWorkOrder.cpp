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
#include "JigsawWorkOrder.h"

#include <QtDebug>

#include <QDockWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "Control.h"
#include "Directory.h"
//#include "JigsawDialog.h"  //tjw
#include "Project.h"

namespace Isis {

  /**
   * This method sets the text of the work order.
   * 
   * @param project The Project that we are going to Bundle Adjust
   * 
   */
  JigsawWorkOrder::JigsawWorkOrder(Project *project) :
      WorkOrder(project) {
    QAction::setText(tr("&Bundle Adjustment..."));
    QUndoCommand::setText("&Bundle Adjustment...");
  }

  /**
   * As of 06/06/2016 this method is not implemented.
   */
  JigsawWorkOrder::JigsawWorkOrder(const JigsawWorkOrder &other) :
      WorkOrder(other) {
  }

  /**
   * Destructor
   */
  JigsawWorkOrder::~JigsawWorkOrder() {
  }

  /**
   * This method clones the JigsawViewWorkOrder
   * 
   * @return @b JigsawWorkOrder Returns a clone of the JigsawWorkOrder
   */
  JigsawWorkOrder *JigsawWorkOrder::clone() const {
    return new JigsawWorkOrder(*this);
  }

  /**
   * This check is used by Directory::supportedActions(DataType data).
   *
   * @return @b bool True if the number of project controls and the number of project images
   *         is greater than 0.
   */
  bool JigsawWorkOrder::isExecutable() {
    return (project()->controls().size() > 0 && project()->images().size() > 0);
  }

  /**
   * If WorkOrder::execute() returns true, this method creates a JigsawDialog.
   * 
   * @return @b bool True if WorkOrder::execute() returns true.
   */
  bool JigsawWorkOrder::execute() {
    bool success = WorkOrder::execute();

    /*  //tjw
    if (success) {


//      QDockWidget* dock = new QDockWidget();
//      dock->setMinimumWidth(525);
//      dock->setMinimumHeight(325);
//      dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        JigsawDialog* bundledlg = new JigsawDialog(project());
        bundledlg->setAttribute(Qt::WA_DeleteOnClose);
        bundledlg->show();
//      dock->setWidget(bundledlg);
//      dock->show();
    }
    */
    return success;
  }

  /**
  * This method returns true if other depends on a JigsawViewWorkOrder
  * 
  * @param order the WorkOrder we want to check for dependancies
  * 
  * @return @b bool True if WorkOrder depends on a JigsawViewWorkOrder
  * 
  */
  bool JigsawWorkOrder::dependsOn(WorkOrder *other) const {
    // depend on types of ourselves.
    return dynamic_cast<JigsawWorkOrder *>(other);
  }

  /**
   * As of 06/06/2016 this method is not implemented as the contents are commented out.
   * 
   */
  void JigsawWorkOrder::syncRedo() {
//    TargetInfoWidget *targetInfoWidget =
//        project()->directory()->addTargetInfoView(targetBody());


//    if (!targetInfoWidget) {
//      QString msg = "error displaying target info";
//      throw IException(IException::Programmer, msg, _FILEINFO_);
//    }
  }

  /**
   * As of 06/06/2016 this method is not implemented as the contents are commented out.
   * 
   */
  void JigsawWorkOrder::syncUndo() {
    //delete project()->directory()->cnetEditorViews().last();
  }
}

