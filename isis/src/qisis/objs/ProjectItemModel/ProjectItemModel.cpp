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

#include "ProjectItemModel.h"

#include <QItemSelection>
#include <QList>
#include <QMimeData>
#include <QModelIndex>
#include <QObject>
#include <QStandardItemModel>
#include <QString>

#include "BundleSolutionInfo.h"
#include "Control.h"
#include "ControlList.h"
#include "GuiCameraList.h"
#include "ImageList.h"
#include "Project.h"
#include "ProjectItem.h"
#include "ShapeList.h"
#include "TargetBodyList.h"

namespace Isis {
  /**
   * Constructs an empty model.
   *
   * @param[in] parent (QObject *) The parent.
   */
  ProjectItemModel::ProjectItemModel(QObject *parent) : QStandardItemModel(parent) {
    m_selectionModel = new QItemSelectionModel(this, this);
    connect(m_selectionModel, SIGNAL(selectionChanged(const QItemSelection &,
                                                      const QItemSelection &) ),
            this, SLOT(onSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
                                    
    connect( this, SIGNAL(rowsInserted(const QModelIndex &, int, int)), 
             this, SLOT(onRowsInserted(const QModelIndex &, int, int)) );

    connect( this, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)), 
             this, SLOT(onRowsRemoved(const QModelIndex &, int, int)) );

  }


  /**
   * Destructs the model.
   */
  ProjectItemModel::~ProjectItemModel() {

  }

  
  /**
   * You cannot drop mime data into the ProjectItemModel. 
   *
   * @see ProjectItemProxyModel
   *
   * @param data (const QMimeData *) The data to drop
   * @param action (Qt::DropAction) The drop action
   * @param row (int) ???
   * @param column (int) ???
   * @param parent (const QModelIndex &) The index of the data's parent
   *
   * @return @b bool False
   */
  bool ProjectItemModel::canDropMimeData(const QMimeData *data,
                                         Qt::DropAction action,
                                         int row, int column,
                                         const QModelIndex &parent) const {
    return false;
  }

  
  /**
   * Returns the internal selection model.
   *
   * @return @b QItemSelectionModel* The selection model.
   */
  QItemSelectionModel *ProjectItemModel::selectionModel() {
    return m_selectionModel;
  }


  /**
   * Adds a Project to the model. An item is created that corresponds to the
   * Project as well as children items that correspond to various parts of the
   * Project.
   *
   * @param[in] project (Project *) The Project to be added.
   *
   * @return @b ProjectItem* The new item that corresponds to the Project.
   */
  ProjectItem *ProjectItemModel::addProject(Project *project) {

    connect(project, SIGNAL( nameChanged(QString) ),
            this, SLOT( onNameChanged(QString) ) );
    connect(project, SIGNAL( bundleSolutionInfoAdded(BundleSolutionInfo *) ),
            this, SLOT( onBundleSolutionInfoAdded(BundleSolutionInfo *) ) );
    connect(project, SIGNAL( controlAdded(Control *) ),
            this, SLOT( onControlAdded(Control *) ) );
    connect(project, SIGNAL( controlListAdded(ControlList *) ),
            this, SLOT( onControlListAdded(ControlList *) ) );
    connect(project, SIGNAL( imagesAdded(ImageList *) ),
            this, SLOT( onImagesAdded(ImageList *) ) );
    connect(project, SIGNAL( shapesAdded(ShapeList *) ),
            this, SLOT( onShapesAdded(ShapeList *) ) );
    connect(project, SIGNAL( targetsAdded(TargetBodyList *) ),
            this, SLOT( onTargetsAdded(TargetBodyList *) ) );
    connect(project, SIGNAL( guiCamerasAdded(GuiCameraList *) ),
            this, SLOT( onGuiCamerasAdded(GuiCameraList *) ) );
    ProjectItem *projectItem = new ProjectItem(project);
    appendRow(projectItem);
//  qDebug()<<"ProjectItem::addProject after appendRow projectItem rowCount = "<<rowCount();
    return projectItem;
  }


  /**
   * Returns the current item of the internal selection model.
   *
   * @return @b ProjectItem* The current item.
   */
  ProjectItem *ProjectItemModel::currentItem() {
    return itemFromIndex( selectionModel()->currentIndex() );
  }


  /**
   * Returns a list of the selected items of the internal selection model.
   *
   * @return @b QList<ProjectItem*> The list of selected items.
   */
  QList<ProjectItem *> ProjectItemModel::selectedItems() {
    QItemSelection selection = selectionModel()->selection();
    QList<ProjectItem *> items;
    
    foreach ( QModelIndex index, selection.indexes() ) {
      items.append( itemFromIndex(index) );
    }

    return items;
  }


  /**
   * Returns the first item found that contains the given data in the given
   * role or a null pointer if no item is found.
   *
   * @param data (const QVariant &) The data contained in the item.
   *
   * @param data (int) The role of the data (see Qt::ItemDataRole).
   *
   * @return @b ProjectItem* First project item found.
   */
  ProjectItem *ProjectItemModel::findItemData(const QVariant &data, int role) {

//  qDebug()<<"ProjectItemModel::findItemData  incoming data = "<<data;
    for (int i=0; i<rowCount(); i++) {
//    qDebug()<<"ProjectItemModel::findItemData  BEFORE call: item(i)->findItemData...";
      ProjectItem *projectItem = item(i)->findItemData(data, role);
//    qDebug()<<"ProjectItemModel::findItemData  AFTER call: item(i)->findItemData...";
      if (projectItem) {
        return projectItem;
      }
    }

    return 0;
  }


  /**
   * Removes an item and its children from the model.
   *
   * @param[in] item (ProjectItem *) The item to be removed.
   */
  void ProjectItemModel::removeItem(ProjectItem *item) {
    if (!item) {
      return;
    }
//  qDebug()<<"ProjectItemModel::removeItem item= "<<item;
    if ( ProjectItem *parentItem = item->parent() ) {
//    qDebug()<<"ProjectItemModel::removeItem  ParentItem ";
      removeRow( item->row(), parentItem->index() );
    }
    else {
//    qDebug()<<"ProjectItemModel::removeItem item row =  "<<item->row();
      removeRow( item->row() );
    }
  }


  /**
   * Removes a list of items and their children from the model.
   *
   * @param[in] item (ProjectItem *) The items to be removed.
   */
  void ProjectItemModel::removeItems(QList<ProjectItem *> items) {
    foreach (ProjectItem *item, items) {
      removeItem(item);
    }
  }
  

  /**
   * Appends a top-level item to the model.
   *
   * @param[in] item (ProjectItem *) The item to append.
   */
  void ProjectItemModel::appendRow(ProjectItem *item) {
    QStandardItemModel::appendRow(item);
  }


  /**
   * Returns the QModelIndex corresponding to a given ProjectItem.
   *
   * @param[in] item (const ProjectItem *) The item.
   *
   * @return @b QModelIndex The index of the item.
   */
  QModelIndex ProjectItemModel::indexFromItem(const ProjectItem *item) {
    return QStandardItemModel::indexFromItem(item);
  }

  
  /**
   * Inserts a top-level item at the given row.
   *
   * @param[in] row (int) The row where the item will be inserted.
   * @param[in] item (ProjectItem *) The item to insert.
   */
  void ProjectItemModel::insertRow(int row, ProjectItem *item) {
    QStandardItemModel::insertRow(row, item);
  }


  /**
   * Returns the top-level item at the given row.
   *
   * @param[in] row (int) The row of the item.
   *
   * @return @b ProjectItem* The item at the row.
   */
  ProjectItem *ProjectItemModel::item(int row) {
    return static_cast<ProjectItem *>( QStandardItemModel::item(row) );
  }


  /**
   * Returns the ProjectItem corresponding to a given QModelIndex.
   *
   * @param[in] index (const QModelIndex &) The index of the item.
   *
   * @return @b ProjectItem* The item.
   */
  ProjectItem *ProjectItemModel::itemFromIndex(const QModelIndex &index) {
    return static_cast<ProjectItem *>( QStandardItemModel::itemFromIndex(index) );
  }

  
  /**
   * Sets the item at the top-level row.
   *
   * @param[in] row (int) The row where the item will be set.
   * @param[in] item (ProjectItem *) The item to set the row to.
   */
  void ProjectItemModel::setItem(int row, ProjectItem *item) {
    QStandardItemModel::setItem(row, item);
  }
  
  
  /**
   * Removes the top-level row and returns the removed item.
   *
   * @param[in] row (int) The row of the item to remove.
   *
   * @return @b ProjectItem* The removed item.
   */
  ProjectItem *ProjectItemModel::takeItem(int row) {
    QList<QStandardItem *> items = QStandardItemModel::takeRow(row);
    
    if ( items.isEmpty() ) {
      return 0;
    }

    return static_cast<ProjectItem *>( items.first() );
  }


  /**
   * Slot to connect to the nameChanged() signal from a Project. Sets the name
   * of the ProjectItem that corresponds with the Project.
   *
   * @param[in] newName (QString) The new name of the project
   */
  void ProjectItemModel::onNameChanged(QString newName) {
    Project *project = qobject_cast<Project *>( sender() );

    if (!project) {
      return;
    }

    for (int i=0; i<rowCount(); i++) {
      ProjectItem *projectItem = item(i);
      if (projectItem->project() == project) {
        projectItem->setText(newName);
      }
    }
  }

  
  /**
   * Slot to connect to the bundleSolutionInfoAdded() signal from a
   * project. Adds a ProjectItem that corresponds to the
   * BundleSolutionInfo to the model. The item is added to the item
   * named "Results" of the item that corresponds to the Project that
   * sent the signal.
   *
   * @param[in] bundleSolutionInfo (BundleSolutionInfo *) The BundleSolutionInfo
   *                                                      added to the Project.
   */
  void ProjectItemModel::onBundleSolutionInfoAdded(BundleSolutionInfo *bundleSolutionInfo) {
    Project *project = qobject_cast<Project *>( sender() );

    if (!project) {
      return;
    }

    for (int i=0; i<rowCount(); i++) {
      ProjectItem *projectItem = item(i);
      if (projectItem->project() == project) {
        for (int j=0; j < projectItem->rowCount(); j++) {
          ProjectItem *resultsItem = projectItem->child(j);
          if (resultsItem->text() == "Results") {
            resultsItem->appendRow( new ProjectItem(bundleSolutionInfo) );
          }
        }
      }
    }
  }


  /**
   * Slot to connect to the controlAdded() signal from a project. Adds
   * a ProjectItem that corresponds to the Control to the model. The
   * item is added to the item that corresponds to the control's
   * ControlList.
   *
   * @param[in] control (Control *) The Control added to the Project.
   */
  void ProjectItemModel::onControlAdded(Control * control) {
    Project *project = qobject_cast<Project *>( sender() );

    if (!project) {
      return;
    }

    for (int i=0; i<rowCount(); i++) {
      ProjectItem *projectItem = item(i);
      if (projectItem->project() == project) {
        for (int j=0; j < projectItem->rowCount(); j++) {
          ProjectItem *controlsItem = projectItem->child(j);
          if (controlsItem->text() == "Control Networks") {
            for (int k=0; k < controlsItem->rowCount(); k++) {
              ProjectItem *controlListItem = controlsItem->child(k);
              ControlList *controlList = controlListItem->controlList();
              if ( controlList && controlList->contains(control) ) {
                controlListItem->appendRow( new ProjectItem(control) );
              }
            }
          }
        }
      }
    }
//  qDebug()<<"ProjectItemModel::onControlAdded rowCount = "<<rowCount();
  }


  /**
   * Slot to connect to the controlListAdded() signal from a
   * Project. Adds a ProjectItem that corresponds to the ControlList
   * to the model. The item is added to the item named "Control
   * Networks" of the item that corresponds to the Project that sent
   * the signal.
   *
   * @param[in] controlList (ControlList *) The ControlList added to the
   *                                        Project.
   */
  void ProjectItemModel::onControlListAdded(ControlList * controlList) {
    Project *project = qobject_cast<Project *>( sender() );

    if (!project) {
      return;
    }

    for (int i=0; i<rowCount(); i++) {
      ProjectItem *projectItem = item(i);
      if (projectItem->project() == project) {
        for (int j=0; j < projectItem->rowCount(); j++) {
          ProjectItem *controlsItem = projectItem->child(j);
          if (controlsItem->text() == "Control Networks") {
            controlsItem->appendRow( new ProjectItem(controlList) );
          }
        }
      }
    }
  }


  /**
   * Slot to connect to the imagesAdded() signal from a Project. Adds
   * a ProjectItem that corresponds to the ImageList to the model. The
   * item is added to the item named "Images" of the item that
   * corresponds to the Project that sent the signal.
   *
   * @param[in] imageList (ImageList *) The ImageList added to the Project.
   */
  void ProjectItemModel::onImagesAdded(ImageList * imageList) {
//  qDebug()<<"ProjectItemModel::onImagesAdded  before add rowCount = "<<rowCount();
    Project *project = qobject_cast<Project *>( sender() );
    
    if (!project) {
      return;
    }
    
    for (int i=0; i<rowCount(); i++) {
      ProjectItem *projectItem = item(i);
      if (projectItem->project() == project) {
        for (int j=0; j < projectItem->rowCount(); j++) {
          ProjectItem *imagesItem = projectItem->child(j);
          if (imagesItem->text() == "Images") {
            imagesItem->appendRow( new ProjectItem(imageList) );
          }
        }
      }
    }
//  qDebug()<<"ProjectItemModel::onImagesAdded  after add rowCount = "<<rowCount();
  }


  /**
   * Slot to connect to the shapesAdded() signal from a Project. Adds a ProjectItem that corresponds
   * to the ShapeList to the model. The item is added to the item named "Shape Models" of the item
   * that corresponds to the Project that sent the signal. 
   *
   * @param[in] shapes (ShapeList *) The ShapeList added to the Project.
   */
  void ProjectItemModel::onShapesAdded(ShapeList * shapes) {
    Project *project = qobject_cast<Project *>( sender() );
    
    if (!project) {
      return;
    }
    for (int i=0; i<rowCount(); i++) {
      ProjectItem *projectItem = item(i);
      if (projectItem->project() == project) {
        for (int j=0; j < projectItem->rowCount(); j++) {
          ProjectItem *shapesItem = projectItem->child(j);
          if (shapesItem->text() == "Shapes") {
            shapesItem->appendRow( new ProjectItem(shapes) );
          }
        }
      }
    }
  }


  /**
   * Slot to connect to the targetsAdded() signal from a Project. Adds
   * items that correspond to the target bodies to the model. The
   * items are added to the item named "Target Body" of the item that
   * corresponds to the Project that sent the signal.
   *
   * @param[in] targets (TargetBodyList *) The TargetBodyList of the Project.
   */
  void ProjectItemModel::onTargetsAdded(TargetBodyList *targets) {
    Project *project = qobject_cast<Project *>( sender() );

    if (!project) {
      return;
    }

    for (int i=0; i<rowCount(); i++) {
      ProjectItem *projectItem = item(i);
      if (projectItem->project() == project) {
        for (int j=0; j < projectItem->rowCount(); j++) {
          ProjectItem *targetsItem = projectItem->child(j);
          if (targetsItem->text() == "Target Body") {
            foreach(TargetBodyQsp target, *targets) {
              bool append = true;
              for (int k=0; k < targetsItem->rowCount(); k++) {
                ProjectItem *targetItem = targetsItem->child(k);
                if (targetItem->targetBody() == target) {
                  append = false;
                }
              }
              if (append) {
                targetsItem->appendRow( new ProjectItem(target) );
              }
            }
          }
        }
      }
    }
  }
  
  
  /**
   * Slot to connect to the guiCamerasAdded() signal from a
   * Project. Adds items that correspond to the cameras to the
   * model. The items are added to the item named "Sensors" of the
   * item that corresponds to the Project that sent the signal.
   *
   * @param[in] cameras (GuiCameraList *) The GuiCameraList of the Project.
   */
  void ProjectItemModel::onGuiCamerasAdded(GuiCameraList *cameras) {
    Project *project = qobject_cast<Project *>( sender() );

    if (!project) {
      return;
    }

    for (int i=0; i<rowCount(); i++) {
      ProjectItem *projectItem = item(i);
      if (projectItem->project() == project) {
        for (int j=0; j < projectItem->rowCount(); j++) {
          ProjectItem *camerasItem = projectItem->child(j);
          if (camerasItem->text() == "Sensors") {
            foreach(GuiCameraQsp camera, *cameras) {
              bool append = true;
              for (int k=0; k < camerasItem->rowCount(); k++) {
                ProjectItem *cameraItem = camerasItem->child(k);
                if (cameraItem->guiCamera() == camera) {
                  append = false;
                }
              }
              if (append) {
                camerasItem->appendRow( new ProjectItem(camera) );
              }
            }
          }
        }
      }
    }
  }


  /**
   * Slot to connect to the selectionChanged() signal from a selection
   * model. Updates other factors in the model that rely on selected
   * items but do not directly correspond with the selection
   * model. Currently changes the selected property of Images that
   * correspond with selected or deselected items.
   *
   * @param[in] selected (const QItemSelection &) The selected items.
   * @param[in] deselected (const QItemSelection &) The deselected items.
   */
  void ProjectItemModel::onSelectionChanged(const QItemSelection &selected,
                                            const QItemSelection &deselected) {
    QList<ProjectItem *> selectedItems;
    foreach ( QModelIndex index, selected.indexes() ) {
      selectedItems.append( itemFromIndex(index) );
    }

    foreach (ProjectItem *item, selectedItems) {
      if ( item->isImage() ) {
        item->image()->displayProperties()->setSelected(true);
      }
    }

    QList<ProjectItem *> deselectedItems;
    foreach ( QModelIndex index, deselected.indexes() ) {
      deselectedItems.append( itemFromIndex(index) );
    }

    foreach (ProjectItem *item, deselectedItems) {
      if ( item->isImage() ) {
        item->image()->displayProperties()->setSelected(false);
      }
    }
  }


  /**
   * Slot to connect to the rowsInserted() signal from
   * QAbstractItemModel. Emits a corresponding itemAdded() signal for
   * each row inserted.
   *
   * @param[in] parent (const QModelIndex &) The parent index where rows
   *                                         were inserted.
   * @param[in] start (int) The first row inserted (inclusive).
   * @param[in] end (int) The last row inserted (inclusive).
   */
  void ProjectItemModel::onRowsInserted(const QModelIndex &parent, int start, int end) {
    for (int row=start; row <= end; row++) {
      QModelIndex newIndex = index(row, 0, parent);
      ProjectItem *item = itemFromIndex(newIndex);
      emit itemAdded(item);
    }
  }


  /**
   * Slot to connect to the rowsAboutToBeRemoved() signal from QAbstractItemModel. Emits a 
   * corresponding itemRemoved() signal for each row inserted. 
   *
   * @param[in] parent (const QModelIndex &) The parent index where rows are to be removed.
   *                                         
   * @param[in] start (int) The first row to be removed (inclusive).
   * @param[in] end (int) The last row to be removed (inclusive).
   */
  void ProjectItemModel::onRowsRemoved(const QModelIndex &parent, int start, int end) {
    for (int row=start; row <= end; row++) {
      QModelIndex newIndex = index(row, 0, parent);
      ProjectItem *item = itemFromIndex(newIndex);
//    qDebug()<<"ProjectItemModel::onRowsRemoved this = "<<this<<"   item = "<<item;
      emit itemRemoved(item);
    }
//  qDebug()<<"ProjectItemModel::onRowsRemoved  Source model : "<<this<<"  row count = "<<rowCount();
  }
}

