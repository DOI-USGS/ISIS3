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
#include "CubeViewportViewWorkOrder.h"

#include <QtDebug>

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "Directory.h"
#include "MdiCubeViewport.h"
#include "Project.h"
#include "Workspace.h"

namespace Isis {

  CubeViewportViewWorkOrder::CubeViewportViewWorkOrder(Project *project) :
      WorkOrder(project) {
    QAction::setText(tr("View Raw &Cubes..."));
  }


  CubeViewportViewWorkOrder::CubeViewportViewWorkOrder(const CubeViewportViewWorkOrder &other) :
      WorkOrder(other) {
  }


  CubeViewportViewWorkOrder::~CubeViewportViewWorkOrder() {
  }


  CubeViewportViewWorkOrder *CubeViewportViewWorkOrder::clone() const {
    return new CubeViewportViewWorkOrder(*this);
  }


  bool CubeViewportViewWorkOrder::isExecutable(ImageList *images) {
    return (images->count() > 0 && images->count() < 50);
  }


  bool CubeViewportViewWorkOrder::execute() {
    bool success = WorkOrder::execute();

    //tjw
    /*
    int maxRecommendedViewports = 10;
    if (success && imageList()->count() > maxRecommendedViewports) {
      QMessageBox::StandardButton selectedOpt = QMessageBox::warning(NULL,
          tr("Potentially Slow Operation"),
          tr("You are asking to open %L1 images' DN data at once in a qview-like view. Working "
             "with more than %L2 cubes in this way is not recommended. Are you sure you want to "
             "view these %L1 cubes?").arg(imageList()->count()).arg(maxRecommendedViewports),
           QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

      if (selectedOpt == QMessageBox::No) {
        success = false;
      }
    }

    if (success) {
      QStringList viewOptions;


      //QList<Workspace *> existingViews = project()->directory()->cubeDnViews();
      int viewToUse = -1;

      if (existingViews.count()) {
        for (int i = 0; i < existingViews.count(); i++) {
          viewOptions.append(existingViews[i]->windowTitle());
        }
      }

      viewOptions.append(tr("New Cube DN View"));

      if (viewOptions.count() > 1) {
        QString selected = QInputDialog::getItem(NULL, tr("View to see cubes in"),
            tr("Which view would you like your\nimage's DN data to be put into?"),
            viewOptions, viewOptions.count() - 1, false, &success);

        viewToUse = viewOptions.indexOf(selected);
      }
      else {
        viewToUse = viewOptions.count() - 1;
      }

      bool newView = false;
      if (viewToUse == viewOptions.count() - 1) {
        newView = true;
        if (!imageList()->name().isEmpty()) {
          QUndoCommand::setText(tr("View image DN data of list [%1] in new cube DN view")
              .arg(imageList()->name()));
        }
        else {
          QUndoCommand::setText(tr("View [%1] image DN data in new cube DN view")
              .arg(imageList()->count()));
        }
      }
      else if (viewToUse != -1) {
        if (!imageList()->name().isEmpty()) {
          QUndoCommand::setText(tr("View image DN data of list [%1] in cube DN view [%2]")
              .arg(imageList()->name()).arg(existingViews[viewToUse]->windowTitle()));
        }
        else {
          QUndoCommand::setText(tr("View [%1] image DN data in cube DN view [%2]")
              .arg(imageList()->count()).arg(existingViews[viewToUse]->windowTitle()));
        }
      }

      QStringList internalData;
      internalData.append(QString::number(viewToUse));
      internalData.append(newView? "new view" : "existing view");
      setInternalData(internalData);
    }
    */
    return success;
  }


  bool CubeViewportViewWorkOrder::dependsOn(WorkOrder *other) const {
    // depend on types of ourselves.
    return dynamic_cast<CubeViewportViewWorkOrder *>(other);
  }


  void CubeViewportViewWorkOrder::syncRedo() {
    //tjw
    /*
    int viewToUse = internalData().first().toInt();

    Workspace *cubeDnViewToUse = NULL;
    if (viewToUse == project()->directory()->cubeDnViews().count()) {
      cubeDnViewToUse = project()->directory()->addCubeDnView();
    }
    else {
      cubeDnViewToUse = project()->directory()->cubeDnViews()[viewToUse];
    }

    cubeDnViewToUse->addImages(imageList());
    */
  }


  void CubeViewportViewWorkOrder::syncUndo() {
    //tjw
    /*
    int viewToUse = internalData().first().toInt();

    if (internalData()[1] == "new view") {
      delete project()->directory()->cubeDnViews().last();
    }
    else {
      Workspace *cubeDnView = project()->directory()->cubeDnViews()[viewToUse];

      QListIterator<Image *> it(*imageList());
      while (it.hasNext()) {
        Image *imageToRemoveFromView = it.next();

        if (cubeDnView->imageToMdiWidget(imageToRemoveFromView))
          cubeDnView->imageToMdiWidget(imageToRemoveFromView)->close();
      }
    }
  */}
}

