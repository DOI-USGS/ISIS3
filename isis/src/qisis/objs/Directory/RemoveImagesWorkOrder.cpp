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
#include "RemoveImagesWorkOrder.h"

#include <QDebug>
#include <QMessageBox>

#include "IException.h"
#include "ImageList.h"
#include "Progress.h"
#include "Project.h"
#include "ProjectItem.h"
#include "ProjectItemModel.h"

namespace Isis {

  RemoveImagesWorkOrder::RemoveImagesWorkOrder(Project *project) :  WorkOrder(project) {

    QAction::setText(tr("&Delete images from project..."));
    QUndoCommand::setText(tr("Delete images from project"));

    setModifiesDiskState(true);
  }


  RemoveImagesWorkOrder::RemoveImagesWorkOrder(const RemoveImagesWorkOrder &other) :
     WorkOrder(other) {
    // This is not undoable
    m_isUndoable = false;
  }


  RemoveImagesWorkOrder::~RemoveImagesWorkOrder() {
  }


  RemoveImagesWorkOrder *RemoveImagesWorkOrder::clone() const {
    return new RemoveImagesWorkOrder(*this);
  }


  /**
   * @brief Determines if we can remove this ImageList
   *
   * @param ImageList *imageList
   * @return bool True if we can remove from project, False otherwise.
   */
  bool RemoveImagesWorkOrder::isExecutable(ImageList *images) {

    return (images->count() > 0);
  }


  /**
   * @description Set up the execution.
   *  
   * @return bool True if parent WordOrder can set up the execution.
   */
  bool RemoveImagesWorkOrder::setupExecution() {

    bool success = WorkOrder::setupExecution();
    return success;
    //TODO: 2016-07-29 TLS Should I ask if files should be deleted from disk also?
  }


  /**
   * @description Remove an item from the project directory.
   */
  void RemoveImagesWorkOrder::execute() {
    qDebug()<<"RemoveImagesWorkOrder::execute  project()->directory()->model() = "<<project()->directory()->model();
    ProjectItem *currentItem = project()->directory()->model()->currentItem();
    project()->directory()->model()->removeItem(currentItem);
//  project()->removeImages(imageList());
  }


  /**
   * @description This method returns false because this WorkOrder is not undoable.
   * 
   * @see WorkOrder::isUndoable()
   * 
   * @return bool Returns false because this WorkOrder is not undoable.
   */
  bool RemoveImagesWorkOrder::isUndoable() {
    
    return false;
  }
}


