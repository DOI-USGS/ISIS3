#include "MosaicTreeWidget.h"

#include <iostream>

#include <QAction>
#include <QDropEvent>
#include <QMenu>
#include <QProgressBar>
#include <QScrollBar>

#include "CubeDisplayProperties.h"
#include "iException.h"
#include "MosaicTreeWidgetItem.h"
#include "ProgressBar.h"

namespace Isis {

  /**
   * MosaicTreeWidget constructor.
   * MosaicTreeWidget is derived from QTreeWidget
   *
   *
   * @param parent
   */
  MosaicTreeWidget::MosaicTreeWidget(QWidget *parent) : QTreeWidget(parent) {
    addGroup("Group1");

    QStringList header;

    MosaicTreeWidgetItem::TreeColumn col = MosaicTreeWidgetItem::NameColumn;
    while(col < MosaicTreeWidgetItem::BlankColumn) {
      header << MosaicTreeWidgetItem::treeColumnToString(col);
      col = (MosaicTreeWidgetItem::TreeColumn)(col + 1);
    }

    setHeaderLabels(header);

    hideColumn(MosaicTreeWidgetItem::ImageColumn);
    hideColumn(MosaicTreeWidgetItem::LabelColumn);
    hideColumn(MosaicTreeWidgetItem::ResolutionColumn);
    hideColumn(MosaicTreeWidgetItem::EmissionAngleColumn);
    hideColumn(MosaicTreeWidgetItem::IncidenceAngleColumn);
    hideColumn(MosaicTreeWidgetItem::BlankColumn);

    setContextMenuPolicy(Qt::DefaultContextMenu);

    setSortingEnabled(true);
    sortItems(MosaicTreeWidgetItem::NameColumn, Qt::AscendingOrder);

    connect(this, SIGNAL(itemSelectionChanged()),
            this, SLOT(updateDragAndDropability()));
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem *, int)),
            this, SLOT(onItemChanged(QTreeWidgetItem *, int)));
    connect(this, SIGNAL(itemSelectionChanged()),
            this, SLOT(onSelectionChanged()));

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragDropOverwriteMode(false);

    p_progress = new ProgressBar;
    p_progress->setVisible(false);
  }


  MosaicTreeWidget::~MosaicTreeWidget() {
    QAction *contextAction;
    foreach(contextAction, actions()) {
      removeAction(contextAction);
    }
  }


  void MosaicTreeWidget::addCubes(QList<CubeDisplayProperties *> cubes) {
    p_progress->setText("Loading file list");
    p_progress->setRange(0, cubes.size() - 1);
    p_progress->setValue(0);
    p_progress->setVisible(true);

    bool initialRefitDone = false;
    QList<QTreeWidgetItem *> newTreeItems;

    CubeDisplayProperties *cube;
    foreach(cube, cubes) {
      MosaicTreeWidgetItem *newTreeItem = prepCube(cube);

      if(newTreeItem) {
        newTreeItems.append(newTreeItem);
      }

      if(newTreeItems.size() > 500) {
        topLevelItem(0)->addChildren(newTreeItems);
        newTreeItems.clear();
      }

      p_progress->setValue(p_progress->value() + 1);

      if(!initialRefitDone) {
        initialRefitDone = true;
        refit();
      }
    }

    topLevelItem(0)->addChildren(newTreeItems);
    newTreeItems.clear();

    p_progress->setVisible(false);
    refit();
  }


  MosaicTreeWidgetItem *MosaicTreeWidget::prepCube(
      CubeDisplayProperties *cube) {
    try {
      MosaicTreeWidgetItem *item = new MosaicTreeWidgetItem(cube);

      connect(cube, SIGNAL(destroyed(QObject *)),
              this, SLOT(cubeDeleted(QObject *)));
      connect(cube, SIGNAL(propertyChanged(CubeDisplayProperties *)),
              this, SLOT(cubeChanged(CubeDisplayProperties *)));

      return item;
    }
    catch(iException &e) {
      e.Report();
      e.Clear();
    }

    return NULL;
  }


  QTreeWidgetItem *MosaicTreeWidget::addGroup(QString groupName, int index) {
    if(index < 0) {
      index = topLevelItemCount();
    }
    else {
      disableSort();
    }

    QTreeWidgetItem *group = new QTreeWidgetItem();
    group->setText(0, groupName);
    insertTopLevelItem(index, group);
    group->setFlags(Qt::ItemIsEditable |
                    Qt::ItemIsUserCheckable |
                    Qt::ItemIsEnabled |
                    Qt::ItemIsSelectable |
                    Qt::ItemIsDragEnabled |
                    Qt::ItemIsDropEnabled);
    group->setExpanded(true);

    verticalScrollBar()->setMaximum(verticalScrollBar()->maximum() + 25);
    return group;
  }


  void MosaicTreeWidget::refit() {
    MosaicTreeWidgetItem::TreeColumn col = MosaicTreeWidgetItem::NameColumn;
    while(col < MosaicTreeWidgetItem::BlankColumn) {
      resizeColumnToContents(col);
      col = (MosaicTreeWidgetItem::TreeColumn)(col + 1);
    }
  }


  QProgressBar *MosaicTreeWidget::getProgress() {
    return p_progress;
  }


  /**
   * This is why we needed to subclass the QTreeWidget class.
   * We needed our own dropEvent for the dragging and dropping
   * of the tree widget items.
   */
  void MosaicTreeWidget::dropEvent(QDropEvent *event) {
    QTreeWidgetItem *droppedAtItem = itemAt(event->pos());

    if(droppedAtItem) {
      disableSort();
      QTreeWidgetItem *droppedAtGroup = droppedAtItem;

      if(droppedAtGroup->type() != QTreeWidgetItem::Type)
        droppedAtGroup = droppedAtItem->parent();

      bool draggedGroup = groupInList(selectedItems());
      bool draggedItem = mosaicItemInList(selectedItems());

      // Moving items around...
      if(draggedItem && !draggedGroup) {
        int insertPosition = 0;

        if(droppedAtGroup != droppedAtItem) {
          insertPosition = droppedAtGroup->indexOfChild(droppedAtItem) + 1;
        }

        QTreeWidgetItem *toBeMoved;
        foreach(toBeMoved, selectedItems()) {
          if(toBeMoved != droppedAtItem) {
            QTreeWidgetItem *parent = toBeMoved->parent();

            // We need to make sure we handle moving a child within the
            //   same group and how that effects the insert position
            int childOrigIndex = parent->indexOfChild(toBeMoved);

            parent->takeChild(childOrigIndex);

            int actualInsertPos = insertPosition;

            if(parent == droppedAtGroup && childOrigIndex < insertPosition)
              actualInsertPos --;

            droppedAtGroup->insertChild(actualInsertPos, toBeMoved);

            // This makes multiple items dragged to bottom of group work
            if(insertPosition != droppedAtGroup->childCount())
              insertPosition ++;
          }
        }
      }
      else if(!draggedItem && draggedGroup) {
        QTreeWidgetItem *toBeMoved;

        foreach(toBeMoved, selectedItems()) {
          if(toBeMoved != droppedAtGroup) {
            int dropPosition = indexOfTopLevelItem(droppedAtGroup);

            takeTopLevelItem(indexOfTopLevelItem(toBeMoved));
            insertTopLevelItem(dropPosition, toBeMoved);
          }
        }
      }
    }
  }


  /**
   * This is re-implemented to make right clicks on white space also unselect
   *   the current selection.
   */
  void MosaicTreeWidget::mousePressEvent(QMouseEvent *event) {
    switch(event->type()) {
      case QEvent::MouseButtonPress: {
        if(event->button() == Qt::RightButton &&
           itemAt(event->pos()) == NULL) {
          setCurrentItem(NULL);
        }
        break;
      }

      default:
        break;
    }

    QTreeWidget::mousePressEvent(event);
  }


  void MosaicTreeWidget::updateDragAndDropability() {
    bool selectedGroup = groupInList(selectedItems());
    bool selectedItem = mosaicItemInList(selectedItems());

    if(selectedGroup && selectedItem) {
      setDragDropMode(QAbstractItemView::NoDragDrop);

    }
    else {
      setDragDropMode(QAbstractItemView::DragDrop);
    }
  }


  void MosaicTreeWidget::contextMenuEvent(QContextMenuEvent *event) {
    bool selectedGroup = groupInList(selectedItems());
    bool selectedCube = mosaicItemInList(selectedItems());

    QMenu menu;

    if(selectedCube || selectedGroup) {
      QList<QAction *> displayActs =
          CubeDisplayProperties::getSupportedDisplayActions(selectedDisplays());

      QAction *displayAct;
      foreach(displayAct, displayActs) {
        menu.addAction(displayAct);
      }

      QList<QAction *> zoomActs =
          CubeDisplayProperties::getSupportedZoomActions(selectedDisplays());

      QList<QAction *> zActs =
          CubeDisplayProperties::getSupportedZOrderActions(selectedDisplays());

      if((zoomActs.size() || zActs.size()) && displayActs.size()) {
        menu.addSeparator();
      }

      QAction *zoomAct;
      foreach(zoomAct, zoomActs) {
        menu.addAction(zoomAct);
      }

      QAction *zAct;
      foreach(zAct, zActs) {
        menu.addAction(zAct);
      }

      menu.addSeparator();
    }

    if(!selectedCube && selectedGroup) {
      if(selectedItems().size() == 1 && selectedItems()[0]->childCount()) {
        QAction *close = menu.addAction("Close Cubes in Group");
        connect(close, SIGNAL(triggered()),
                this, SLOT(requestCloseSelected()));
      }

      if(selectedItems().size() == 1) {
        QAction *rename = menu.addAction("Rename Group");
        connect(rename, SIGNAL(triggered()),
                this, SLOT(renameSelectedGroup()));

        QAction *group = menu.addAction("Insert Group");
        connect(group, SIGNAL(triggered()),
                this, SLOT(addGroup()));
      }

      QAction *removeGroup = NULL;

      if(selectedItems().size() == 1)
        removeGroup = menu.addAction("Delete Group");
      else
        removeGroup = menu.addAction("Delete Groups");

      connect(removeGroup, SIGNAL(triggered()),
              this, SLOT(deleteSelectedGroups()));
    }

    if(selectedCube && !selectedGroup) {
      QAction *close = NULL;

      if(selectedItems().size() == 1)
        close = menu.addAction("Close Cube");
      else
        close = menu.addAction("Close Cubes");

      connect(close, SIGNAL(triggered()),
              this, SLOT(requestCloseSelected()));
    }

    if(!selectedCube && !selectedGroup) {
      QAction *group = menu.addAction("Add Group");

      connect(group, SIGNAL(triggered()),
              this, SLOT(addGroup()));
    }

    if(!selectedCube) {
      menu.addSeparator();

      QList<QAction *> viewActs = getViewActions();

      QAction *viewAct;
      foreach(viewAct, viewActs) {
        menu.addAction(viewAct);
      }
    }

    menu.exec(event->globalPos());
  }


  void MosaicTreeWidget::requestCloseSelected() {
    QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::UserFlag);

    while(*it) {
      QTreeWidgetItem *item = *it;

      // We get at least one item of the wrong type
      if(item->type() == QTreeWidgetItem::UserType) {
        if(item->isSelected() || item->parent()->isSelected()) {
          MosaicTreeWidgetItem *mosItem = (MosaicTreeWidgetItem *)item;
          mosItem->cubeDisplay()->deleteLater();
        }
      }

      it ++;
    }
  }


  QTreeWidgetItem *MosaicTreeWidget::addGroup() {
    int index = -1;

    if(selectedItems().size() == 1 &&
       selectedItems()[0]->type() == QTreeWidgetItem::Type) {
      QTreeWidgetItem *itemBelowNew = selectedItems()[0];
      index = indexOfTopLevelItem(itemBelowNew);
    }

    QString newGroupName("Group" +
        QString::number(topLevelItemCount() + 1));

    return addGroup(newGroupName, index);
  }


  void MosaicTreeWidget::cubeDeleted(QObject *cubeDispObj) {
    CubeDisplayProperties *cubeDisplay = (CubeDisplayProperties *)cubeDispObj;
    MosaicTreeWidgetItem *item = treeItem(cubeDisplay);

    if(item && item->parent()) {
      item->forgetDisplay();
      delete item->parent()->takeChild( item->parent()->indexOfChild(item) );
    }
  }


  void MosaicTreeWidget::cubeChanged(CubeDisplayProperties *changed) {
    MosaicTreeWidgetItem *item = treeItem(changed);
    if(item) {
      bool wasSelected = item->isSelected();
      item->update(false);

      if(!wasSelected && item->isSelected())
        scrollToItem(item);
    }
  }


  void MosaicTreeWidget::deleteSelectedGroups() {
    // Close the cubes in these groups
    requestCloseSelected();

    QTreeWidgetItem *toBeDeleted;
    foreach(toBeDeleted, selectedItems()) {
      if(toBeDeleted->type() == QTreeWidgetItem::Type) {
        delete toBeDeleted;
        toBeDeleted = NULL;
      }
    }
  }

  void MosaicTreeWidget::onItemChanged(QTreeWidgetItem *item, int) {
    if(item->type() == QTreeWidgetItem::UserType) {
      ((MosaicTreeWidgetItem *)item)->update(true);
    }
  }


  void MosaicTreeWidget::onSelectionChanged() {
    QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::UserFlag);

    while(*it) {
      QTreeWidgetItem *item = *it;

      if(item->type() == QTreeWidgetItem::UserType) {
        MosaicTreeWidgetItem *mosItem = (MosaicTreeWidgetItem *)item;
        mosItem->update(true);
      }

      it ++;
    }
  }


  void MosaicTreeWidget::renameSelectedGroup() {
    if(selectedItems().size() == 1 &&
       selectedItems()[0]->type() == QTreeWidgetItem::Type) {
      QTreeWidgetItem *groupToEdit = selectedItems()[0];
      editItem(groupToEdit);
    }
  }


  void MosaicTreeWidget::toggleColumnVisible() {
    int column = ((QAction *)sender())->data().toInt();
    setColumnHidden(column, !isColumnHidden(column));

    if(!p_viewActs.empty())
      updateViewActs();

    refit();
  }


  bool MosaicTreeWidget::groupInList(QList<QTreeWidgetItem *> items) {
    QTreeWidgetItem *item;
    foreach(item, items) {
      if(item->type() == QTreeWidgetItem::Type)
        return true;
    }

    return false;
  }


  bool MosaicTreeWidget::mosaicItemInList(QList<QTreeWidgetItem *> items) {
    QTreeWidgetItem *item;
    foreach(item, items) {
      if(item->type() == QTreeWidgetItem::UserType)
        return true;
    }

    return false;
  }

  QList<CubeDisplayProperties *> MosaicTreeWidget::selectedDisplays() {
    QList<CubeDisplayProperties *> selected;
    QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::UserFlag);

    while(*it) {
      QTreeWidgetItem *item = *it;

      // We get at least one item of the wrong type
      if(item->type() == QTreeWidgetItem::UserType) {
        MosaicTreeWidgetItem *mosItem = (MosaicTreeWidgetItem *)item;
        if(mosItem->isSelected() || mosItem->parent()->isSelected()) {
          selected.append(mosItem->cubeDisplay());
        }
      }

      it ++;
    }

    return selected;
  }

  void MosaicTreeWidget::updateViewActs() {
    if(!p_viewActs.empty()) {
      int viewActIndex = 0;
      MosaicTreeWidgetItem::TreeColumn col =
          MosaicTreeWidgetItem::FootprintColumn;

      while(col < MosaicTreeWidgetItem::BlankColumn) {
        bool visible = !isColumnHidden(col);
        QAction *showHide = p_viewActs[viewActIndex];

        if(!visible) {
          showHide->setText("Show " +
              MosaicTreeWidgetItem::treeColumnToString(col) + " Column");
        }
        else {
          showHide->setText("Hide " +
              MosaicTreeWidgetItem::treeColumnToString(col) + " Column");
        }

        col = (MosaicTreeWidgetItem::TreeColumn)(col + 1);
        viewActIndex ++;
      }
    }
  }


  void MosaicTreeWidget::disableSort() {
    sortItems(MosaicTreeWidgetItem::BlankColumn, Qt::AscendingOrder);
  }


  QList<QAction *> MosaicTreeWidget::getViewActions() {
    if(p_viewActs.empty()) {
      MosaicTreeWidgetItem::TreeColumn col =
          MosaicTreeWidgetItem::FootprintColumn;

      while(col < MosaicTreeWidgetItem::BlankColumn) {
        QAction *showHide = new QAction("", this);
        showHide->setData(col);

        connect(showHide, SIGNAL(triggered()),
                this, SLOT(toggleColumnVisible()));

        p_viewActs.append(showHide);

        col = (MosaicTreeWidgetItem::TreeColumn)(col + 1);
      }

      updateViewActs();
    }

    return p_viewActs;
  }


  MosaicTreeWidgetItem *MosaicTreeWidget::treeItem(
      CubeDisplayProperties *cubeDisplay) {
    MosaicTreeWidgetItem *result = NULL;

    QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::UserFlag);

    while(*it && !result) {
      QTreeWidgetItem *item = *it;

      // We get at least one item of the wrong type
      if(item->type() == QTreeWidgetItem::UserType) {
        MosaicTreeWidgetItem *mosItem = (MosaicTreeWidgetItem *)item;
        if(mosItem->cubeDisplay() == cubeDisplay) {
          result = mosItem;
        }
      }

      it ++;
    }

    return result;
  }
}

