/**
 * @file
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
#include "Footprint2DViewWorkOrder.h"

#include <QtDebug>

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "Directory.h"
#include "Footprint2DView.h"
#include "ImageList.h"
#include "MosaicSceneItem.h"
#include "MosaicSceneWidget.h"
#include "Project.h"
#include "ProjectItem.h"
#include "ProjectItemModel.h"

namespace Isis {
  
  /** 
   * Creates a work order to view image footprints. This WorkOrder is not undoable and runs 
   * synchronously.
   * 
   * @param Project *project Pointer to the project this work order belongs to.
   */
  Footprint2DViewWorkOrder::Footprint2DViewWorkOrder(Project *project) :
      WorkOrder(project) {
    m_isUndoable = false;
    QAction::setText(tr("View &Footprints..."));
  }


  /**
   * @brief Copy constructor.
   *
   * Creates a copy of the other Footprint2DViewWorkOrder.
   *
   * @param Footprint2DViewWorkOrder &other The other work order to copy state from.
   */
  Footprint2DViewWorkOrder::Footprint2DViewWorkOrder(const Footprint2DViewWorkOrder &other) :
      WorkOrder(other) {
    m_isUndoable = other.m_isUndoable;
  }


  /**
   * @brief Destructor.
   *
   * Destructor to clean up any memory that this work order allocates.
   */
   Footprint2DViewWorkOrder::~Footprint2DViewWorkOrder() {
  }


  /**
   * @brief This method clones the current Footprint2DViewWorkOrder and returns it.
   * 
   * @return @b Footprint2DViewWorkOrder that was cloned
   */
  Footprint2DViewWorkOrder *Footprint2DViewWorkOrder::clone() const {

    return new Footprint2DViewWorkOrder(*this);
  }
  

  /**
   * @brief This method returns true if one of an image in ImageList images isFootprintable, 
   *              False if none of the images has a footprint. This is used by
   *              Directory::supportedActions(DataType data) to determine what actions are appended
   *              to context menus.
   * 
   * @param images ImageList of images
   * 
   * @return @b bool True if one of the images in ImagesList images isFootprintable
   */
  bool Footprint2DViewWorkOrder::isExecutable(ImageList *images) {
    bool result = false;

    foreach (Image *image, *images) {
      result = result || image->isFootprintable();
    }

    return result;
  }


  /**
   * @brief This method returns true if one of the shapes in ShapeList isFootprintable, False 
   *              if none of shapes have a footprint.  This is used by
   *              Directory::supportedActions(DataType data) to determine what actions are appended
   *              to context menus.
   *
   * @param shapes ShapeList we are checking
   *
   * @return @b bool True if one of the shapes in ShapeList images isFootprintable
   */
  bool Footprint2DViewWorkOrder::isExecutable(ShapeList *shapes) {
    bool result = false;

    foreach (Shape *shape, *shapes) {
      result = result || shape->isFootprintable();
    }

    return result;
  }


  /**
   * @brief Setup this WorkOrder for execution.  Prompt for whether these footprints should be
   *              displayed in a new view or an existing view.  This calls
   *              WorkOrder::setupExecution().
   * 
   * @return @b bool True if WorkOrder::execute() returns true and footprints can be created, 
   *         otherwise returns False.
   */
  bool Footprint2DViewWorkOrder::setupExecution() {
    bool success = WorkOrder::setupExecution();

    int maxRecommendedFootprints = 50000;
    if (success && imageList()->count() > maxRecommendedFootprints) {
      QMessageBox::StandardButton selectedOpt = QMessageBox::warning(NULL,
          tr("Potentially Slow Operation"),
          tr("You are asking to open %L1 images in a 2D footprint view at once. This is possible, "
             "but will take a significant amount of time and cause overall slowness. Working with "
             "more than %L2 footprints is not recommended. Are you sure you want to view these "
             "%L1 footprints?").arg(imageList()->count()).arg(maxRecommendedFootprints),
           QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

      if (selectedOpt == QMessageBox::No) {
        success = false;
      }
    }

    int viewToUse = -1;
    if (success) {
      QStringList viewOptions;

      QList<Footprint2DView *> existingViews = project()->directory()->footprint2DViews();
      if (existingViews.count()) {
        for (int i = 0; i < existingViews.count(); i++) {
          viewOptions.append(existingViews[i]->windowTitle());
        }
      }

      viewOptions.append(tr("New Footprint View"));

      if (viewOptions.count() > 1) {
        QString selected = QInputDialog::getItem(NULL, tr("View to see footprints in"),
            tr("Which view would you like your\nimage's footprints to be put into?"),
            viewOptions, viewOptions.count() - 1, false, &success);

        viewToUse = viewOptions.indexOf(selected);
      }
      else {
        viewToUse = viewOptions.count() - 1;
      }

      if (viewToUse == viewOptions.count() - 1) {
        QUndoCommand::setText(tr("View footprints in new 2D footprint view"));
      }
      else if (viewToUse != -1) {
         QUndoCommand::setText(tr("View footprints in footprint view [%1]")
              .arg(existingViews[viewToUse]->windowTitle()));
      }

      QStringList internalData;
      internalData.append(QString::number(viewToUse));
      setInternalData(internalData);
    }
    return success;
  }


  /**
   * @brief This either adds a new Footprint2DView containing the selected images or adds the 
   *              image's footprints to an existing Footprint2DView.
   *  
   */
  void Footprint2DViewWorkOrder::execute() {

    QList<ProjectItem *> selectedItems = project()->directory()->model()->selectedItems();

    int viewToUse = internalData().first().toInt();

    Footprint2DView *view = NULL;
    if (viewToUse == project()->directory()->footprint2DViews().count()) {
      view = project()->directory()->addFootprint2DView();
    }
    else {
      view = project()->directory()->footprint2DViews()[viewToUse];
    }

    view->addItems( selectedItems );
  }
}

