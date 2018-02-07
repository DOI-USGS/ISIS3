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

    m_isUndoable = false;

    QAction::setText("&Delete images from project...");

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
   *
   */
  bool RemoveImagesWorkOrder::setupExecution() {

    bool success = WorkOrder::setupExecution();

    QList<ProjectItem *> selectedItems = project()->directory()->model()->selectedItems();
    long itemCount = selectedItems.count();
    if (itemCount == 1) {
      if (selectedItems.at(0)->isImage()) {
        QUndoCommand::setText("&Delete image " + selectedItems.at(0)->image()->fileName() + " from project...");
      }
      else if (selectedItems.at(0)->isImageList()) {
        QUndoCommand::setText("&Delete " + QString::number(selectedItems.at(0)->imageList()->count(), 10) + " images from project...");
      }
    }
    else {
      long totalCount = 0;
      QList<ImageList *> imageListAdded;
      ImageList images;
      foreach(ProjectItem *item, selectedItems) {
        if (item->isImage()) {
          totalCount++;
          images.append(item->image());
        }
        else if (item->isImageList()) {
          totalCount += item->imageList()->count();
          imageListAdded.append(item->imageList());
        }
      }
      // this is needed to be able to get a correct count when a user selects an image and
      // the imagelist containing that image
      foreach (ImageList *imageList, imageListAdded) {
        foreach (Image *image, images) {
          if (imageList->contains(image)) {
            totalCount--;
          }
        }
      }
        QUndoCommand::setText("&Delete " + QString::number(totalCount, 10) + " images from project...");
    }

    return success;
    //TODO: 2016-07-29 TLS Should I ask if files should be deleted from disk also?
  }


  /**
   * @description Remove any selected items from the project directory.
   *
   * @internal
   *   @history 2017-08-08 Marjorie Hahn - Created a conditional to check if the currently
   *                           selected item to be deleted is actually an image list, so that
   *                           each image can be removed one by one. Fixes #5074.
   *   @history 2017-10-04 Cole Neubauer - Moved ImageList to its own loop so images are deleted
   *                           first and we avoid the issue of attempting to delete an image that
   *                           is gone
   */
  void RemoveImagesWorkOrder::execute() {

    QList<ProjectItem *> selectedItems = project()->directory()->model()->selectedItems();
    QList<ProjectItem *> needToRemoveItems;


    QList<ImageList *> projectImageLists = project()->images();
    QList<ImageList *> imageListToProcess;
    foreach (ProjectItem *selectedItem, selectedItems) {
      if (selectedItem->isImage()) {
        Image *selectedImage = selectedItem->image();
        project()->directory()->model()->removeItem(selectedItem);
        
        foreach (ImageList* projectImageList, projectImageLists) {
          projectImageList->removeAll(selectedImage);
        }
      }
      else if (selectedItem->isImageList()) {
        imageListToProcess.append(selectedItem->imageList());
        needToRemoveItems.append(selectedItem);
      }
      else {
        throw IException(IException::User,
                         "Item cannot be removed from the project.",
                         _FILEINFO_);
      }
    }
    foreach(ImageList *imageList, imageListToProcess) {
      foreach (Image *selectedImage, *imageList) {
        foreach (ImageList* projectImageList, projectImageLists) {
          projectImageList->removeAll(selectedImage);
        }
      }
      project()->removeImages(*imageList);
    }

    project()->directory()->model()->removeItems(needToRemoveItems);
    project()->setClean(false);
  }
}
