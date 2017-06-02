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
#include "ImageFileListViewWorkOrder.h"

#include <QDebug>
#include <QInputDialog>

#include "Directory.h"
#include "Project.h"
#include "ImageFileListWidget.h"

namespace Isis {

  ImageFileListViewWorkOrder::ImageFileListViewWorkOrder(Project *project) :
      WorkOrder(project) {
    QAction::setText(tr("View File &Names..."));
  }


  ImageFileListViewWorkOrder::ImageFileListViewWorkOrder(const ImageFileListViewWorkOrder &other) :
      WorkOrder(other) {
  }


  ImageFileListViewWorkOrder::~ImageFileListViewWorkOrder() {
  }


  ImageFileListViewWorkOrder *ImageFileListViewWorkOrder::clone() const {
    return new ImageFileListViewWorkOrder(*this);
  }


  bool ImageFileListViewWorkOrder::isExecutable(ImageList *images) {
    return !images->isEmpty();
  }
  
  
  /**
   * @brief This method returns false because this WorkOrder is not undoable.
   * 
   * @see WorkOrder::isUndoable()
   * 
   * @return bool Returns false because this WorkOrder is not undoable.
   */
  bool ImageFileListViewWorkOrder::isUndoable() const {
    return false;
  }

  
  /**
   * @brief This method asks the user what view they want to see their image list in. The user 
   * can select an existing view or they can create a new view. The user's choice is then saved 
   * using setInternalData(). 
   * 
   * @see WorkOrder::setupExecution()
   * 
   * @return @b bool True if WorkOrder::setupExecution() returns true.
   */
  bool ImageFileListViewWorkOrder::setupExecution() {
    bool success = WorkOrder::setupExecution();

    if (success) {
      QStringList viewOptions;

      QList<ImageFileListWidget *> existingViews = project()->directory()->imageFileListViews();
      int viewToUse = -1;

      if (existingViews.count()) {
        for (int i = 0; i < existingViews.count(); i++) {
          viewOptions.append(existingViews[i]->windowTitle());
        }
      }

      viewOptions.append(tr("New File List View"));

      if (viewOptions.count() > 1) {
        QString selected = QInputDialog::getItem(NULL, tr("View to see files in"),
            tr("Which view would you like your\nimage's file names to be put into?"),
            viewOptions, viewOptions.count() - 1, false, &success);

        viewToUse = viewOptions.indexOf(selected);
      }
      else {
        viewToUse = viewOptions.count() - 1;
      }

      if (viewToUse == viewOptions.count() - 1) {
        if (!imageList()->name().isEmpty()) {
          QUndoCommand::setText(tr("View image file names of list [%1] in new file list view")
              .arg(imageList()->name()));
        }
        else {
          QUndoCommand::setText(tr("View [%1] Image File Names in new file list view")
              .arg(imageList()->count()));
        }
      }
      else if (viewToUse != -1) {
        if (!imageList()->name().isEmpty()) {
          QUndoCommand::setText(tr("View image file names of list [%1] in file list view [%2]")
              .arg(imageList()->name()).arg(existingViews[viewToUse]->windowTitle()));
        }
        else {
          QUndoCommand::setText(tr("View [%1] Image File Names in file list view [%2]")
              .arg(imageList()->count()).arg(existingViews[viewToUse]->windowTitle()));
        }
      }

      QStringList internalData;
      internalData.append(QString::number(viewToUse));
      setInternalData(internalData);
    }

    return success;
  }

  
  /**
   * @brief This method adds a new image file list to the project's directory and then adds 
   * currentItem() to that. 
   */
  void ImageFileListViewWorkOrder::execute() {
    int viewToUse = internalData().first().toInt();

    ImageFileListWidget *fileListToUse = NULL;
    if (viewToUse == project()->directory()->imageFileListViews().count()) {
      fileListToUse = project()->directory()->addImageFileListView();
    }
    else {
      fileListToUse = project()->directory()->imageFileListViews()[viewToUse];
    }

    fileListToUse->addImages(imageList());
  }
}
