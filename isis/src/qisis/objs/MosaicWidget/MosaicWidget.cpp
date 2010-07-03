#include <cfloat>
#include <QContextMenuEvent>
#include <QColorDialog>
#include <QSizePolicy>

#include "MosaicWidget.h"
#include "MosaicItem.h"
#include "MosaicMainWindow.h"
#include "FileDialog.h"
#include "MosaicZoomTool.h"
#include "MosaicPanTool.h"
#include "MosaicSelectTool.h"
#include "MosaicTrackTool.h"
#include "MosaicPointTool.h"
#include "MosaicControlNetTool.h"
#include "MosaicFindTool.h"

#include "Pvl.h"
#include "PvlGroup.h"
#include "ControlGraph.h"
#include "iString.h"


namespace Qisis {

  /**
   * MosaicWidget constructor.
   * MosaicWidget is derived from QSplitter, the left side of the
   * splitter is a QTreeWidget and the right side of the splitter
   * is a QGraphicsView.
   *
   *
   * @param parent
   */
  MosaicWidget::MosaicWidget(QWidget *parent) : QSplitter(Qt::Horizontal, parent) {

    p_projection = 0;
    p_insertItemAt = -1;
    p_parent = (MosaicMainWindow *)parent;
    p_textItem = NULL;
    p_rubberBand = NULL;
    p_cn = NULL;
    p_footprintItem = new QGraphicsPolygonItem();
    p_footprintItem->hide();

    QFont font("Helvetica", 10, QFont::Normal);
    setFont(font);
    installEventFilter(this);
    initWidget();
  }

  /**
   * Initializes all the parts required for the MosaicWidget.
   *
   */
  void MosaicWidget::initWidget() {

    p_mapFileButton = new QToolButton();
    p_mapFileButton->setToolTip("Select Map File");
    p_mapFileButton->setText("Select Map File");
    p_mapFileButton->setIcon(QPixmap
                             (QString::fromStdString(Isis::Filename("$base/icons").Expanded().c_str()) + "/mIconProjectionEnabled.png"));
    p_parent->permanentToolBar()->addWidget(p_mapFileButton);
    connect(p_mapFileButton, SIGNAL(pressed()), this, SLOT(setMapFile()));

    p_mapDisplay = new QLabel(p_parent);
    p_mapDisplay->setText("Select a map file.");
    p_parent->permanentToolBar()->addWidget(p_mapDisplay);

    p_parent->permanentToolBar()->addSeparator();

    p_controlPointButton = new QToolButton();
    p_controlPointButton->setToolTip("Display Control Points");
    p_controlPointButton->setCheckable(true);
    p_controlPointButton->setIcon(QPixmap
                                  (QString::fromStdString(Isis::Filename("$base/icons").Expanded().c_str()) + "/HILLBLU_molecola.png"));
    //p_parent->permanentToolBar()->addWidget(p_controlPointButton);
    //connect(p_controlPointButton, SIGNAL(pressed()), this, SLOT(displayControlPoints()));

    p_connectivityButton = new QToolButton();
    p_connectivityButton->setToolTip("Display Network Connectivity");
    p_connectivityButton->setCheckable(true);
    //p_parent->permanentToolBar()->addWidget(p_connectivityButton);
    //connect(p_connectivityButton, SIGNAL(pressed()), this, SLOT(displayConnectivity()));

    p_treeWidget = new MosaicTreeWidget(this);
    connect(p_treeWidget, SIGNAL(itemDropped(QPoint)), this, SLOT(dropAction(QPoint)));
    //connect(p_treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(itemChanged(QTreeWidgetItem *, int)));
    connect(p_treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(groupChanged(QTreeWidgetItem *, int)));
    p_treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    p_treeWidget->setDragDropMode(QAbstractItemView::InternalMove);
    p_treeWidget->setDragDropOverwriteMode(false);
    p_treeWidget->setColumnCount(6);
    p_treeWidget->installEventFilter(this);

    p_graphicsScene = new QGraphicsScene(this);
    p_graphicsScene->installEventFilter(this);
    p_graphicsView = new QGraphicsView(p_graphicsScene, this);
    p_graphicsView->setScene(p_graphicsScene);
    p_graphicsView->setContextMenuPolicy(Qt::NoContextMenu);
    p_graphicsView->setOptimizationFlag(QGraphicsView::DontSavePainterState, true);
    p_graphicsView->setInteractive(true);

    addWidget(p_treeWidget);
    addWidget(p_graphicsView);

    QSizePolicy treePolicy = p_treeWidget->sizePolicy();
    treePolicy.setHorizontalStretch(2);
    p_treeWidget->setSizePolicy(treePolicy);
    QSizePolicy graphicsViewPolicy = p_graphicsView->sizePolicy();
    graphicsViewPolicy.setHorizontalStretch(255);
    p_graphicsView->setSizePolicy(graphicsViewPolicy);

    addGroup("Group1");

    QStringList header;
    header << "Name" << "Item" << "Footprint" << "Outline" << "Image"
           << "Label" << "Resolution" << "Emission Angle"
           << "Incidence Angle" << "Island" << "Notes";
    p_treeWidget->setHeaderLabels(header);
    p_treeWidget->setColumnWidth(NameColumn, 160); //Name
    p_treeWidget->setColumnWidth(ItemColumn, 35); //Item
    p_treeWidget->setColumnWidth(FootprintColumn, 60); //Footprint
    p_treeWidget->setColumnWidth(OutlineColumn, 50); //Outline
    p_treeWidget->setColumnWidth(ImageColumn, 45); //Image
    p_treeWidget->setColumnWidth(LabelColumn, 40); //Label
    p_treeWidget->setColumnWidth(ResolutionColumn, 70); //Resolution
    p_treeWidget->setColumnWidth(EmissionColumn, 100); //Emission Angle
    p_treeWidget->setColumnWidth(IncidenceColumn, 110); //Incidence Angle
    p_treeWidget->setColumnWidth(IslandColumn, 45); //Island Number

    p_xmin = DBL_MAX;
    p_xmax = -DBL_MAX;
    p_ymin = DBL_MAX;
    p_ymax = -DBL_MAX;

    //Adding the select tool
    p_stool = new MosaicSelectTool(this);
    p_stool->setGraphicsView(p_graphicsView);
    p_stool->addTo(p_parent);
    p_stool->activate(true);

    //Adding the zoom tool.
    p_ztool = new MosaicZoomTool(this);
    p_ztool->setGraphicsView(p_graphicsView);
    p_ztool->addTo(p_parent);

    //Adding the pan tool.
    p_ptool = new MosaicPanTool(this);
    p_ptool->setGraphicsView(p_graphicsView);
    p_ptool->addTo(p_parent);

    //Adding the track tool.
    p_ttool = new MosaicTrackTool(p_parent->statusBar());
    p_ttool->setGraphicsView(p_graphicsView);
    p_ttool->setWidget(this);
    p_ttool->addTo(p_parent);

    //Adding the point tool.
    p_pntool = new MosaicPointTool(this);
    p_pntool->setGraphicsView(p_graphicsView);
    p_pntool->setWidget(this);
    p_pntool->addTo(p_parent);

    //Adding the control net tool.
    p_cntool = new MosaicControlNetTool(this);
    p_cntool->setGraphicsView(p_graphicsView);
    p_cntool->setWidget(this);
    p_cntool->addTo(p_parent);

    //Adding the find tool.
    p_ftool = new MosaicFindTool(this);
    p_ftool->setGraphicsView(p_graphicsView);
    p_ftool->setWidget(this);
    p_ftool->addTo(p_parent);

    connect(p_treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)),
            this, SLOT(updateGraphicsView(QTreeWidgetItem *, int)));

    connect(p_treeWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(updateGraphicsView()));

    connect(p_graphicsScene, SIGNAL(selectionChanged()),
            this, SLOT(updateTreeWidget()));

    //------------------------------------------------------
    // This sets up the columns the user wants visible
    // in the tree widget.  These settings were remembered
    // from the last time the user ran qmos.
    //------------------------------------------------------
    QMenu *viewMenu = p_parent->viewMenu();
    QList <QAction *> actions = viewMenu->actions();
    for(int i = 0; i < actions.size(); i++) {
      viewMenuAction(actions[i]);
    }
  }


  /**
   * Adds a new Top Level Item to the tree widget.
   *
   * @param groupName
   */
  void MosaicWidget::addGroup(const QString &groupName) {
    //Test to see if we already have a group with the same name
    if(p_groupToTreeMap.contains(groupName)) return;

    QTreeWidgetItem *group = new QTreeWidgetItem();
    group->setText(0, groupName);
    p_treeWidget->addTopLevelItem(group);
    group->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    group->setExpanded(true);
    p_groupToTreeMap.insert(groupName, group);
  }


  /**
   * Adds a new topLevelItem to the tree widget
   *
   */
  void MosaicWidget::addGroup() {
    QString newGroupName("Group" + QString::number(p_treeWidget->topLevelItemCount() + 1));
    addGroup(newGroupName);
  }

  /**
   * Deletes the group with the name passed in as 'groupName'.
   *
   * @param groupName
   */
  void MosaicWidget::deleteGroup(const QString &groupName) {
    QList <QTreeWidgetItem *> groupItems = p_treeWidget->findItems(groupName, Qt::MatchExactly);
    if(groupItems.size() == 0)return;
    QTreeWidgetItem *group = groupItems.last();
    for(int j = 0; j < group->childCount(); j++) {
      int childIndex = group->indexOfChild(group->child(j));
      group->takeChild(childIndex);
    }
    int index = p_treeWidget->indexOfTopLevelItem(group);
    p_treeWidget->takeTopLevelItem(index);
    p_groupToTreeMap.erase(p_groupToTreeMap.find(groupName));

  }


  /**
   *  Gets rid of an entire group and it's items, then will
   *  refit the remaining items to the graphics scene.
   */
  void MosaicWidget::deleteGroup() {
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    hideItem();
    if(groupItems.isEmpty()) return;
    for(int i = 0; i < groupItems.size(); i++) {
      QTreeWidgetItem *currentGroup = groupItems[i];
      for(int j = 0; j < currentGroup->childCount(); j++) {
        int childIndex = currentGroup->indexOfChild(currentGroup->child(j));
        currentGroup->takeChild(childIndex);
      }
      int index = p_treeWidget->indexOfTopLevelItem(currentGroup);
      p_treeWidget->takeTopLevelItem(index);
      p_groupToTreeMap.erase(p_groupToTreeMap.find(currentGroup->text(0)));
    }
    refit();
  }


  /**
   * Adds the mosaicItem to the graphicsScene.
   *
   * @param mosaicItem
   * @param groupName
   */
  void MosaicWidget::addItem(MosaicItem *mosaicItem, QString groupName) {
    // check if this mosaicItem has already been added to the scene.
    for(int i = 0; i < p_mosaicItems.size(); i++) {

      if(mosaicItem->filename() == p_mosaicItems[i]->filename() && !p_pasteItems.contains(mosaicItem)) {
        std::cout << "WARNING: This cube is already in the scene!" << std::endl;
        return;
      }
    }

    // If the groupName does not already exist then add it.
    if(!p_groupToTreeMap.contains(groupName)) {
      addGroup(groupName);
    }

    // Get the x/y min/max of the item so the numbers can be used in fitInView
    if(mosaicItem->XMinimum() < p_xmin) p_xmin = mosaicItem->XMinimum();
    if(mosaicItem->YMinimum() < p_ymin) p_ymin = mosaicItem->YMinimum();
    if(mosaicItem->XMaximum() > p_xmax) p_xmax = mosaicItem->XMaximum();
    if(mosaicItem->YMaximum() > p_ymax) p_ymax = mosaicItem->YMaximum();

    QTreeWidgetItem *groupItem = p_groupToTreeMap[groupName];
    if(p_insertItemAt == -1) {
      groupItem->addChild(mosaicItem->treeItem());
      mosaicItem->treeItem()->setText(ResolutionColumn, QString::number(mosaicItem->pixelResolution()));
      mosaicItem->treeItem()->setText(EmissionColumn, QString::number(mosaicItem->emissionAngle()));
      mosaicItem->treeItem()->setText(IncidenceColumn, QString::number(mosaicItem->incidenceAngle()));
    }
    else {
      groupItem->insertChild(p_insertItemAt, mosaicItem->treeItem());
      mosaicItem->treeItem()->setText(ResolutionColumn, QString::number(mosaicItem->pixelResolution()));
      mosaicItem->treeItem()->setText(EmissionColumn, QString::number(mosaicItem->emissionAngle()));
      mosaicItem->treeItem()->setText(IncidenceColumn, QString::number(mosaicItem->incidenceAngle()));
    }
    p_treeToMosaicMap.insert(mosaicItem->treeItem(), mosaicItem);
    mosaicItem->setZValue(-FLT_MAX);
    setInitialZValue(groupItem);
    p_graphicsScene->addItem(mosaicItem);
    if(mosaicItem->children().size() > 0) {
      mosaicItem->children().first()->installSceneEventFilter(mosaicItem);
      mosaicItem->installSceneEventFilter(mosaicItem->children().first());
    }

    //--------------------------------------------------------------------------
    // Everytime we add an item we want to make sure everything fits in the view
    // Unless we are cutting and pasting.
    //--------------------------------------------------------------------------
    if(p_pasteItems.size() < 1)
      p_graphicsView->fitInView(p_xmin - 5, p_ymin - 5, (p_xmax - p_xmin) + 5, (p_ymax - p_ymin) + 5, Qt::KeepAspectRatio);

    // If this items is not already in our list, then we need to add it.
    if(!p_mosaicItems.contains(mosaicItem)) {
      if(p_insertItemAt == -1) {
        p_mosaicItems.push_back(mosaicItem);
      }
      else {
        p_mosaicItems.insert(p_insertItemAt, mosaicItem);
      }
    }

    // reset p_insertItemAt
    p_insertItemAt = -1;

    // display the screen resolution in the zoom tool's p_scaleBox
    p_screenResolution = p_graphicsScene->width() / p_graphicsView->viewport()->width();
    if(p_ztool->isActive()) {
      p_ztool->updateResolutionBox();
    }

    //-----------------------------------------------------------------------
    // if this item has some control points in it from the control net, and
    // the points are visible, then we need to make sure they get displayed.
    //-----------------------------------------------------------------------
    if(p_cn != NULL) {
      if(p_controlPointButton->isChecked()) {
        mosaicItem->displayControlPoints(p_cn);
      }
    }

    //if(p_showReference->checkState() == Qt::Checked) {
    // if(!p_graphicsScene->items().contains(p_footprintItem)) {
    // createReferenceFootprint();
    //}
    //}

    return;
    //TODO: Throw error if group not found.
  }


  /**
   * This is an overloaded method for the one above.
   * This is called from the open method.
   *
   * @param itemName
   */
  void MosaicWidget::addItem(QString itemName) {
    Qisis::MosaicItem *mosItem = new Qisis::MosaicItem(itemName, this);
    // If a group was not selected, just add the item to the bottom group.
    if(selectedGroups().isEmpty()) {
      int numGroups = p_treeWidget->topLevelItemCount();
      addItem(mosItem, p_treeWidget->topLevelItem(numGroups - 1)->text(0));
    }
    else {
      addItem(mosItem, selectedGroups().first()->text(0));
    }
  }


  /**
   * This method creates the reference footprint if defined in the map file.
   *
   */
  void MosaicWidget::createReferenceFootprint() {
    double x = 0;
    double y = 0;

    QVector<QPointF> footprintPoints;
    QPolygonF footprintPoly;

    // need to create a polygon from the min/max lat/long numbers in the map file.
    try {
      Isis::Pvl pvl;
      pvl.Read(p_mapfile.toStdString());
      Isis::PvlKeyword minLatKeyword = pvl.FindKeyword("MinimumLatitude", Isis::Pvl::Traverse);
      QString minLat = QString::fromStdString(minLatKeyword[0]);
      Isis::PvlKeyword minLongKeyword = pvl.FindKeyword("MinimumLongitude", Isis::Pvl::Traverse);
      QString minLon = QString::fromStdString(minLongKeyword[0]);
      Isis::PvlKeyword maxLatKeyword = pvl.FindKeyword("MaximumLatitude", Isis::Pvl::Traverse);
      QString maxLat = QString::fromStdString(maxLatKeyword[0]);
      Isis::PvlKeyword maxLongKeyword = pvl.FindKeyword("MaximumLongitude", Isis::Pvl::Traverse);
      QString maxLon = QString::fromStdString(maxLongKeyword[0]);

      if(p_projection->SetUniversalGround(minLat.toDouble(), minLon.toDouble())) {
        x = p_projection->XCoord();
        y = -1 * (p_projection->YCoord());
        footprintPoints.push_back(QPointF(x, y));
      }

      for(double lat = minLat.toDouble() + 1; lat < maxLat.toDouble(); lat++) {
        if(p_projection->SetUniversalGround(lat, minLon.toDouble())) {
          x = p_projection->XCoord();
          y = -1 * (p_projection->YCoord());
          footprintPoints.push_back(QPointF(x, y));
        }
      }
      for(double lon = minLon.toDouble() + 1; lon < maxLon.toDouble(); lon++) {
        if(p_projection->SetUniversalGround(maxLat.toDouble(), lon)) {
          x = p_projection->XCoord();
          y = -1 * (p_projection->YCoord());
          footprintPoints.push_back(QPointF(x, y));
        }
      }
      for(double lat = maxLat.toDouble(); lat > minLat.toDouble() + 1; lat--) {
        if(p_projection->SetUniversalGround(lat, maxLon.toDouble())) {
          x = p_projection->XCoord();
          y = -1 * (p_projection->YCoord());
          footprintPoints.push_back(QPointF(x, y));
        }
      }
      for(double lon = maxLon.toDouble(); lon > minLon.toDouble() + 1; lon--) {
        if(p_projection->SetUniversalGround(minLat.toDouble(), lon)) {
          x = p_projection->XCoord();
          y = -1 * (p_projection->YCoord());
          footprintPoints.push_back(QPointF(x, y));
        }
      }

      //Now close the polygon.
      if(p_projection->SetUniversalGround(minLat.toDouble(), minLon.toDouble())) {
        x = p_projection->XCoord();
        y = -1 * (p_projection->YCoord());
        footprintPoints.push_back(QPointF(x, y));
      }
      footprintPoly = QPolygonF(footprintPoints);
      p_footprintItem->setPolygon(footprintPoly);
      p_footprintItem->setBrush(QBrush(QColor(255, 255, 0, 100)));
      p_footprintItem->setPen(QColor(Qt::black));
      p_footprintItem->setZValue(-FLT_MAX);
      p_footprintItem->setFlag(QGraphicsItem::ItemIsSelectable, false);
      p_graphicsScene->addItem(p_footprintItem);
      //p_graphicsView->fitInView(p_footprintItem, Qt::KeepAspectRatio);
      p_footprintItem->show();

    }
    catch(Isis::iException &e) {
      std::string msg = e.Errors();
      QMessageBox::information(this, "Error", QString::fromStdString(msg), QMessageBox::Ok);
      //p_showReference->setChecked(Qt::Unchecked);
      return;
    }
  }


  /**
   * hides or shows the reference footprint
   *
   */
  void MosaicWidget::setReferenceItemVisible(bool show) {
    if(show) {
      if(!p_graphicsScene->items().contains(p_footprintItem)) {
        createReferenceFootprint();
      }
      else {
        p_footprintItem->show();
      }
    }
    else {
      p_footprintItem->hide();
    }
  }


  /**
   * This method assigns the initial zValue to a mosaic item
   * which is determined by it's child index number in it's parent
   * group.
   *
   * @param groupItem
   */
  void MosaicWidget::setInitialZValue(QTreeWidgetItem *groupItem) {
    int index = p_treeWidget->indexOfTopLevelItem(groupItem) * -10000;
    MosaicItem *mosItem;

    for(int i = 0; i < groupItem->childCount(); i++) {
      mosItem = p_treeToMosaicMap[groupItem->child(i)];
      if(mosItem->zValue() == -FLT_MAX) {
        mosItem->setZValue(index - i);
      }
    }
  }


  /**
   * When a treeWidget item is moved to a new position on the
   * tree, the zValue for that items is set to -FLT_MAX.  This
   * method assigns the correct new zValue to the item.
   *
   * @param groupItem
   */
  void MosaicWidget::fixZValue(QTreeWidgetItem *groupItem) {
    if(groupItem->childCount() < 2) return;
    qreal zValue = -FLT_MAX;

    MosaicItem *mosItem;
    qreal topZValue =  -FLT_MAX;
    qreal bottomZValue =  -FLT_MAX;
    for(int i = 0; i < groupItem->childCount(); i++) {
      mosItem = p_treeToMosaicMap[groupItem->child(i)];

      if(mosItem->zValue() == -FLT_MAX) {

        // Check to see if mosItem is at the top
        if(mosItem == p_treeToMosaicMap[groupItem->child(0)]) {
          zValue = p_treeToMosaicMap[groupItem->child(1)]->zValue() + 1;
        }
        // Check to see if the mosItem is at the bottom
        else if(mosItem == p_treeToMosaicMap[groupItem->child(groupItem->childCount()-1)]) {
          zValue = p_treeToMosaicMap[groupItem->child(groupItem->childCount()-2)]->zValue() - 1;
        }
        else {
          //-------------------------------------------------------------
          // mosItem is not the top item OR the bottom item.
          // get the zvalue of the item above and the item below this one
          //-------------------------------------------------------------
          if(i > 0) {
            topZValue = p_treeToMosaicMap[groupItem->child(i-1)]->zValue();
          }
          if(i < groupItem->childCount() - 1) {
            bottomZValue = p_treeToMosaicMap[groupItem->child(i+1)]->zValue();
          }
          zValue = (topZValue + bottomZValue) / 2;
        }

        //TODO: check to see if the zValue is within the range of the zValues of
        // the previous or next group.    IF so, then call reorderAllZValues.

        if(zValue != -FLT_MAX) mosItem->setZValue(zValue);
      }// end if mosItem->zValue == -FLT_MAX
    }
  }


  /**
   * This is a slot connected to the tree widgets signal
   * "itemChanged(QTreeWidgetItem *, int)"
   * Sets the mosaicItem visible or not depending on the tree
   * widget item's check state.
   *
   * @param item
   * @param column
   */
  void MosaicWidget::updateGraphicsView(QTreeWidgetItem *item, int column) {
    if(!p_treeToMosaicMap.contains(item)) return;

    if(column == 1) {
      p_treeToMosaicMap[item]->setItemVisible(
        (item->checkState(column) == Qt::Checked) ? true : false);
      p_treeToMosaicMap[item]->setSelected(
        (item->checkState(column) == Qt::Checked) ? true : false);
    }

    if(column == FootprintColumn ||
        column == OutlineColumn ||
        column == ImageColumn ||
        column == LabelColumn) {
      updateGraphicsView();
      p_treeToMosaicMap[item]->update();
    }

  }


  /**
   * This is a slot connected to the tree widget's signal
   * "itemSelectionChanged()"
   *
   */
  void MosaicWidget::updateGraphicsView() {
    //-----------------------------------------------------
    // We must disconnect the signals of the graphics scene
    // to avoid getting into an endless loop
    //-----------------------------------------------------
    disconnect(p_graphicsScene, SIGNAL(selectionChanged()),
               this, SLOT(updateTreeWidget()));
    p_graphicsScene->clearSelection();
    for(int i = 0; i < p_mosaicItems.size(); i++) {
      if((!p_mosaicItems[i]->isSelected() && p_mosaicItems[i]->isTreeItemSelected()) ||
          (p_mosaicItems[i]->isSelected() && !p_mosaicItems[i]->isTreeItemSelected())) {
        p_mosaicItems[i]->setSelected(p_mosaicItems[i]->isTreeItemSelected());
      }
    }
    // Re-connect the graphics scene
    connect(p_graphicsScene, SIGNAL(selectionChanged()),
            this, SLOT(updateTreeWidget()));
  }


  /**
   * This is a slot connect to the graphics scene's signal
   * "selectionChanged()"
   *
   */
  void MosaicWidget::updateTreeWidget() {
    //-------------------------------------------------
    // We must disconnect the signals of the tree widget
    // to avoid getting into an endless loop
    //------------------------------------------------
    disconnect(p_treeWidget, SIGNAL(itemSelectionChanged()),
               this, SLOT(updateGraphicsView()));

    p_treeWidget->clearSelection();
    for(int i = 0; i < p_mosaicItems.size(); i++) {
      p_mosaicItems[i]->setTreeItemSelected(p_mosaicItems[i]->isSelected());
    }
    // Re-connect the tree widget
    connect(p_treeWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(updateGraphicsView()));
  }


  /**
   * This method determines which menu action items should be
   * displayed on the menu depending on what the user right
   * clicked on the obtain the context menu.
   *
   * @return QMenu
   */
  QMenu *MosaicWidget::contextMenu() {
    static QMenu *mainContextMenu = 0;
    static QMenu *sortMenu;
    static QAction *insertCube;
    static QAction *insertList;
    static QAction *deleteCube;
    static QAction *addGroup;
    static QAction *mergeGroups;
    static QAction *deleteGroup;
    static QAction *renameGroup;
    static QAction *toFront;
    static QAction *sendBack;
    static QAction *moveUpOne;
    static QAction *moveDownOne;
    static QAction *changeColor;
    static QAction *changeTransparency;
    static QAction *hideImageAction;
    static QAction *showImageAction;
    static QAction *hideItemAction;
    static QAction *showItemAction;
    static QAction *hideLabelAction;
    static QAction *showLabelAction;
    static QAction *hideOutlineAction;
    static QAction *showOutlineAction;
    static QAction *hideFootprintAction;
    static QAction *showFootprintAction;
    static QAction *zoomToItemAction;
    static QAction *resolutionSort;
    static QAction *emissionAngleSort;
    static QAction *incidenceAngleSort;
    static QAction *islandSort;
    static QAction *cut;
    static QAction *paste;
    static QAction *setLabelFont;

    if(mainContextMenu == 0) {
      insertCube = new QAction("Insert Cube", this);
      connect(insertCube, SIGNAL(triggered()),
              this, SLOT(insertCube()));

      insertList = new QAction("Import List", this);
      connect(insertList, SIGNAL(triggered()),
              this, SLOT(openList()));

      deleteCube = new QAction("Delete Cube", this);
      deleteCube->setShortcut(Qt::Key_Delete);
      connect(deleteCube, SIGNAL(triggered()),
              this, SLOT(deleteCube()));

      resolutionSort = new QAction("Resolution", this);
      connect(resolutionSort, SIGNAL(triggered()),
              this, SLOT(sortByResolution()));

      emissionAngleSort = new QAction("Emission Angle", this);
      connect(emissionAngleSort, SIGNAL(triggered()),
              this, SLOT(sortByEmission()));

      incidenceAngleSort = new QAction("Incidence Angle", this);
      connect(incidenceAngleSort, SIGNAL(triggered()),
              this, SLOT(sortByIncidence()));

      islandSort = new QAction("Island", this);
      connect(islandSort, SIGNAL(triggered()),
              this, SLOT(sortByIsland()));

      addGroup = new QAction("Add Group", this);
      connect(addGroup, SIGNAL(triggered()),
              this, SLOT(addGroup()));

      mergeGroups = new QAction("Merge Groups", this);
      connect(mergeGroups, SIGNAL(triggered()),
              this, SLOT(mergeGroups()));

      deleteGroup = new QAction("Delete Group", this);
      connect(deleteGroup, SIGNAL(triggered()),
              this, SLOT(deleteGroup()));

      renameGroup = new QAction("Rename Group", this);
      connect(renameGroup, SIGNAL(triggered()),
              this, SLOT(renameGroup()));

      toFront = new QAction("Bring to Front", this);
      connect(toFront, SIGNAL(triggered()),
              this, SLOT(bringToFront()));

      sendBack = new QAction("Send to Back", this);
      connect(sendBack, SIGNAL(triggered()),
              this, SLOT(sendToBack()));

      moveUpOne = new QAction("Move Up One", this);
      connect(moveUpOne, SIGNAL(triggered()),
              this, SLOT(moveUpOne()));

      moveDownOne = new QAction("Move Down One", this);
      connect(moveDownOne, SIGNAL(triggered()),
              this, SLOT(moveDownOne()));

      changeColor = new QAction("Change Footprint Color/Opacity", this);
      connect(changeColor, SIGNAL(triggered()),
              this, SLOT(changeColor()));

      changeTransparency = new QAction("Change Image Opacity", this);
      connect(changeTransparency, SIGNAL(triggered()),
              this, SLOT(changeTransparency()));

      hideItemAction = new QAction("Hide Item(s)", this);
      connect(hideItemAction, SIGNAL(triggered()),
              this, SLOT(hideItem()));

      showItemAction = new QAction("Show Item(s)", this);
      connect(showItemAction, SIGNAL(triggered()),
              this, SLOT(showItem()));

      hideImageAction = new QAction("Hide Image(s)", this);
      connect(hideImageAction, SIGNAL(triggered()),
              this, SLOT(hideImage()));

      showImageAction = new QAction("Show Image(s)", this);
      connect(showImageAction, SIGNAL(triggered()),
              this, SLOT(showImage()));

      hideLabelAction = new QAction("Hide Label(s)", this);
      connect(hideLabelAction, SIGNAL(triggered()),
              this, SLOT(hideLabel()));

      showLabelAction = new QAction("Show Label(s)", this);
      connect(showLabelAction, SIGNAL(triggered()),
              this, SLOT(showLabel()));

      hideOutlineAction = new QAction("Hide Outline(s)", this);
      connect(hideOutlineAction, SIGNAL(triggered()),
              this, SLOT(hideOutline()));

      showOutlineAction = new QAction("Show Outline(s)", this);
      connect(showOutlineAction, SIGNAL(triggered()),
              this, SLOT(showOutline()));

      hideFootprintAction = new QAction("Hide Footprint(s)", this);
      connect(hideFootprintAction, SIGNAL(triggered()),
              this, SLOT(hideFootprint()));

      showFootprintAction = new QAction("Show Footprint(s)", this);
      connect(showFootprintAction, SIGNAL(triggered()),
              this, SLOT(showFootprint()));

      zoomToItemAction = new QAction("Zoom To Item", this);
      connect(zoomToItemAction, SIGNAL(triggered()),
              this, SLOT(zoomToItem()));

      cut = new QAction("Cut", this);
      connect(cut, SIGNAL(triggered()),
              this, SLOT(cut()));

      paste = new QAction("Paste", this);
      connect(paste, SIGNAL(triggered()),
              this, SLOT(paste()));

      setLabelFont = new QAction("Set Label Font", this);
      connect(setLabelFont, SIGNAL(triggered()),
              this, SLOT(setLabelFont()));

      mainContextMenu = new QMenu("Context Menu");
      sortMenu = new QMenu("Sort by:");

      sortMenu->addAction(resolutionSort);
      sortMenu->addAction(emissionAngleSort);
      sortMenu->addAction(incidenceAngleSort);
      sortMenu->addAction(islandSort);

      mainContextMenu->addAction(insertCube);
      mainContextMenu->addAction(insertList);
      mainContextMenu->addAction(deleteCube);
      mainContextMenu->addAction(cut);
      mainContextMenu->addAction(paste);

      mainContextMenu->addSeparator();

      mainContextMenu->addAction(toFront);
      mainContextMenu->addAction(sendBack);
      mainContextMenu->addAction(moveUpOne);
      mainContextMenu->addAction(moveDownOne);
      mainContextMenu->addAction(zoomToItemAction);

      mainContextMenu->addSeparator();

      mainContextMenu->addAction(changeColor);
      mainContextMenu->addAction(changeTransparency);
      mainContextMenu->addAction(hideItemAction);
      mainContextMenu->addAction(showItemAction);

      mainContextMenu->addSeparator();

      mainContextMenu->addAction(hideImageAction);
      mainContextMenu->addAction(showImageAction);

      mainContextMenu->addSeparator();

      mainContextMenu->addAction(hideFootprintAction);
      mainContextMenu->addAction(showFootprintAction);

      mainContextMenu->addSeparator();

      mainContextMenu->addAction(hideLabelAction);
      mainContextMenu->addAction(showLabelAction);

      mainContextMenu->addSeparator();

      mainContextMenu->addAction(setLabelFont);

      mainContextMenu->addSeparator();

      mainContextMenu->addAction(hideOutlineAction);
      mainContextMenu->addAction(showOutlineAction);

      mainContextMenu->addSeparator();

      mainContextMenu->addMenu(sortMenu);
      mainContextMenu->addAction(addGroup);
      mainContextMenu->addAction(mergeGroups);
      mainContextMenu->addAction(deleteGroup);
      mainContextMenu->addAction(renameGroup);
    }

    sortMenu->setEnabled(false);
    addGroup->setEnabled(false);
    mergeGroups->setEnabled(false);
    deleteGroup->setEnabled(false);
    renameGroup->setEnabled(false);
    insertCube->setEnabled(false);
    insertList->setEnabled(false);
    deleteCube->setEnabled(false);
    changeColor->setEnabled(false);
    changeTransparency->setEnabled(false);
    toFront->setEnabled(false);
    sendBack->setEnabled(false);
    moveUpOne->setEnabled(false);
    moveDownOne->setEnabled(false);
    zoomToItemAction->setEnabled(false);
    hideItemAction->setEnabled(false);
    showItemAction->setEnabled(false);
    hideImageAction->setEnabled(false);
    showImageAction->setEnabled(false);
    hideLabelAction->setEnabled(false);
    showLabelAction->setEnabled(false);
    hideOutlineAction->setEnabled(false);
    showOutlineAction->setEnabled(false);
    hideFootprintAction->setEnabled(false);
    showFootprintAction->setEnabled(false);
    cut->setEnabled(false);
    paste->setEnabled(false);
    setLabelFont->setEnabled(false);

    // User has selected a group tree item
    if(!selectedGroups().isEmpty()) {
      addGroup->setEnabled(true);
      if(selectedGroups().size() > 1) mergeGroups->setEnabled(true);
      deleteGroup->setEnabled(true);
      sortMenu->setEnabled(true);
      insertCube->setEnabled(true);
      insertList->setEnabled(true);
      //if(selectedGroups().size() == 1) renameGroup->setEnabled(true);
      renameGroup->setEnabled(true);
      changeColor->setEnabled(true);
      changeTransparency->setEnabled(true);
      hideItemAction->setEnabled(true);
      showItemAction->setEnabled(true);
      hideImageAction->setEnabled(true);
      showImageAction->setEnabled(true);
      hideLabelAction->setEnabled(true);
      showLabelAction->setEnabled(true);
      hideOutlineAction->setEnabled(true);
      showOutlineAction->setEnabled(true);
      hideFootprintAction->setEnabled(true);
      showFootprintAction->setEnabled(true);
      setLabelFont->setEnabled(true);
      // Only allow a paste option if there has been a cut performed.
      if(p_pasteItems.size() > 0) paste->setEnabled(true);
    }

    // User has selected a mosaic tree item
    if(!selectedMosaicItems().isEmpty()) {
      changeColor->setEnabled(true);
      changeTransparency->setEnabled(true);
      hideItemAction->setEnabled(true);
      showItemAction->setEnabled(true);
      hideImageAction->setEnabled(true);
      showImageAction->setEnabled(true);
      hideLabelAction->setEnabled(true);
      showLabelAction->setEnabled(true);
      hideOutlineAction->setEnabled(true);
      showOutlineAction->setEnabled(true);
      hideFootprintAction->setEnabled(true);
      showFootprintAction->setEnabled(true);
      deleteCube->setEnabled(true);
      insertCube->setEnabled(true);
      insertList->setEnabled(true);
      cut->setEnabled(true);
      setLabelFont->setEnabled(true);
    }

    //---------------------------------------------------------------------
    // User has selected one and only one mosaic item or one and only one
    // tree item.
    //--------------------------------------------------------------------
    if((selectedMosaicItems().size() == 1 && selectedGroups().size() == 0) ||
        (selectedMosaicItems().size() == 0 && selectedGroups().size() == 1)) {
      toFront->setEnabled(true);
      sendBack->setEnabled(true);
      moveUpOne->setEnabled(true);
      moveDownOne->setEnabled(true);
      if(selectedGroups().size() == 0) zoomToItemAction->setEnabled(true);
    }
    return mainContextMenu;
  }


  /**
   * When a contextMenuEvent (right click) takes place on the
   * widget, the context menu is popped up with the correct
   * options depending on what widget the event took place.
   *
   * @param event
   */
  void MosaicWidget::contextMenuEvent(QContextMenuEvent *event) {
    if(p_ztool->isActive()) return;

    if(p_projection == NULL) {
      p_textItem = p_graphicsScene->addText("Please select a map file first!");
      return;
    }
    // determine if the right click happened in the tree widget
    if(p_treeWidget->rect().contains(p_treeWidget->mapFromGlobal(event->globalPos()))) {
      contextMenu()->popup(event->globalPos());
      event->accept();
    }

    // determine if the right click happened in the graphics scene
    if(p_graphicsView->rect().contains(p_graphicsView->mapFromGlobal(event->globalPos()))) {
      QGraphicsItem *item = p_graphicsView->itemAt(p_graphicsView->mapFromGlobal(event->globalPos()));
      p_treeWidget->clearSelection();
      if(item != 0) {
        item->setSelected(true);
        contextMenu()->popup(event->globalPos());
      }
      event->accept();
    }
  }


  /**
   * Hides the selected footprint and removes the filename from
   * the tree widget.  Then refit() fits the remaining footprints
   * into the scene properly.
   */
  void MosaicWidget::deleteCube() {
    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    hideItem();
    for(int i = 0; i < mosaicItems.size(); i++) {
      MosaicItem *item = mosaicItems[i];
      p_mosaicItems.removeAt(p_mosaicItems.indexOf(mosaicItems[i]));
      QTreeWidgetItem *group = item->treeItem()->parent();
      if(group != 0) {
        int index = group->indexOfChild(item->treeItem());
        group->takeChild(index);
      }
    }
  }


  /**
   * Deletes all the mosaic items out of qmos.
   * This is useful when the user opens a new project
   * when there already mosaic items open.
   */
  void MosaicWidget::deleteAllCubes() {
    if(p_mosaicItems.size() > 0) {
      for(int i = 0; i < p_mosaicItems.size(); i++) {
        p_mosaicItems[i]->setItemVisible(false);
        QTreeWidgetItem *group = p_mosaicItems[i]->treeItem()->parent();
        if(group != 0) {
          int index = group->indexOfChild(p_mosaicItems[i]->treeItem());
          group->takeChild(index);
          p_treeWidget->takeTopLevelItem(p_treeWidget->indexOfTopLevelItem(group));
          p_groupToTreeMap.take(group->text(0));
        }
      }
      p_mosaicItems.clear();
    }
  }


  /**
   * Allows the user to insert a cube in the order of their
   * choice.
   *
   */
  void MosaicWidget::insertCube() {
    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    if(!mosaicItems.isEmpty()) {
      insertCube(selectedMosaicItems().first());
    }
    else {
      open();
    }
  }


  /**
   * Allows the user to insert a cube in the order of their
   * choice.
   *
   * @param item
   */
  void MosaicWidget::insertCube(MosaicItem *item) {
    // calculate what to set p_insertItemAt;
    int index = item->treeItem()->parent()->indexOfChild(item->treeItem());
    item->treeItem()->parent()->setSelected(true);
    p_insertItemAt = index + 1;
    open();
  }


  /**
   * This method opens a cube
   *
   */
  void MosaicWidget::open() {
    if(p_projection == NULL) {
      p_textItem = p_graphicsScene->addText("Please select a map file first!");
      return;
    }
    if(p_treeWidget->selectedItems().size() < 1) {
      int numGroups = p_treeWidget->topLevelItemCount();
      p_treeWidget->topLevelItem(numGroups - 1)->setSelected(true);
    }
    if(!p_filterList.contains("Isis cubes (*.cub)")) {
      p_filterList.append("Isis cubes (*.cub)");
    }
    p_filterList.append("All Files (*)");
    if(!p_dir.exists()) {
      p_dir = QDir::current();
    }

    FileDialog *fileDialog = new FileDialog("Open", p_filterList, p_dir,
                                            (QWidget *)parent());
    fileDialog->show();

    connect(fileDialog, SIGNAL(fileSelected(QString)),
            this, SLOT(addItem(QString)));
  }


  /**
   * Opens a list of cubes
   *
   */
  void MosaicWidget::openList() {
    if(p_projection == NULL) {
      p_textItem = p_graphicsScene->addText("Please select a map file first!");
      return;
    }
    if(p_treeWidget->selectedItems().size() < 1) {
      int numGroups = p_treeWidget->topLevelItemCount();
      p_treeWidget->topLevelItem(numGroups - 1)->setSelected(true);
    }
    // Set up the list of filters that are default with this dialog.
    if(!p_filterList2.contains("List Files (*.lis)")) {
      p_filterList2.append("List Files (*.lis)");
      p_filterList2.append("Text Files (*.txt)");
      p_filterList2.append("All files (*)");
    }
    if(!p_dir.exists()) {
      p_dir = QDir::current();
    }
    FileDialog *fileDialog = new FileDialog("Import List", p_filterList2, p_dir,
                                            (QWidget *)parent());
    fileDialog->show();

    connect(fileDialog, SIGNAL(fileSelected(QString)),
            this, SLOT(readFile(QString)));
  }


  /**
   * This method reads a file containing a list a cube filenames
   * and opens each one.
   *
   * @param listFile
   */
  void MosaicWidget::readFile(QString listFile) {
    QFile f(listFile);
    Isis::Filename fn(listFile.toStdString());

    f.open(QIODevice::ReadOnly);
    QString str = f.readLine();
    while(str.length() != 0) {
      if(!str.startsWith("#")) {
        if(str.endsWith("-group\n")) {
          //------------------------------------------
          // Chop off the -group part that gets added
          // in the saveList method.
          //------------------------------------------
          str.chop(7);
          addGroup(str);
        }
        else {
          //------------------------------------------------
          // Be sure to only select the last group so that
          // the new item gets added to the correct group.
          //------------------------------------------------
          int numGroups = p_treeWidget->topLevelItemCount();
          for(int i = 0; i < numGroups; i++) {
            p_treeWidget->topLevelItem(i)->setSelected(false);
          }
          p_treeWidget->topLevelItem(numGroups - 1)->setSelected(true);
          addItem(str);
        }
      }
      str = f.readLine();

    }
    f.close();
  }


  /**
   * This slot is called when a mosaicItem or tree widget group
   * has been requested to bring to front.  This slot figures out
   * which kind of bringToFront needs to happen, then called the
   * appropriate method.
   *
   */
  void MosaicWidget::bringToFront() {
    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    if(!mosaicItems.isEmpty()) {
      bringToFront(selectedMosaicItems().first());
    }
    else {
      bringToFront(selectedGroups().first());
    }

    //----------------------------------------------------------
    // Need to re-order the items in the p_mosaicItems list
    // so that when the project gets saved, it will be saved in
    // the correct order.
    //-----------------------------------------------------------
    reorderMosaicItemsList();
  }


  /**
   * This method is called from the bringToFront slot. a mosaic
   * item has been requested to bring to the front.  The item is
   * brought to the front of it's group.  fixZvalue is called to
   * change the z value of all the items in that group.
   *
   * @param item
   */
  void MosaicWidget::bringToFront(MosaicItem *item) {
    QTreeWidgetItem *group = item->treeItem()->parent();
    int index = group->indexOfChild(item->treeItem());

    // This re-orders the tree widget.
    group->takeChild(index);
    group->insertChild(0, item->treeItem());

    // This will set the z-values of the items.
    item->setZValue(-FLT_MAX);
    fixZValue(group);
  }


  /**
  * This method is called from the brintToFront slot. a tree
  * widget group has been requested to be brought to the front.
  * reorderAllZValues is then called to change the z value of all
  * the items in the view.
  *
  * @param group
  */
  void MosaicWidget::bringToFront(QTreeWidgetItem *group) {
    int index = p_treeWidget->indexOfTopLevelItem(group);
    bool expanded = group->isExpanded();
    p_treeWidget->takeTopLevelItem(index);
    p_treeWidget->insertTopLevelItem(0, group);
    group->setExpanded(expanded);
    reorderAllZValues();
  }


  /**
   * This method re-sets all the z-values for all the items in the
   * view.
   *
   */
  void MosaicWidget::reorderAllZValues() {
    for(int i = 0; i < p_treeWidget->topLevelItemCount(); i++) {
      reorderGroupZValues(p_treeWidget->topLevelItem(i));
    }
  }


  /**
   * This method re-sets the the z-values for all the items within
   * the group.
   *
   * @param groupItem
   */
  void MosaicWidget::reorderGroupZValues(QTreeWidgetItem *groupItem) {
    for(int j = 0; j < groupItem->childCount(); j++) {
      p_treeToMosaicMap[groupItem->child(j)]->setZValue(-FLT_MAX);
    }
    setInitialZValue(groupItem);
  }


  /**
   * This method reorders the p_mosaicItems QList so that it
   * always matches what the user has arranged in the QTreeWidget.
   * (i.e. the list on the left side of the qmos window.)
   *
   */
  void MosaicWidget::reorderMosaicItemsList() {
    // copy all the mosaic items into a temp. hold area.
    QList <MosaicItem *> tempItems = p_mosaicItems;
    int insertAt = 0;
    int totalChildCount = 0;

    for(int i = 0; i < p_treeWidget->topLevelItemCount(); i++) {
      for(int k = 0; k < tempItems.size(); k++) {
        for(int j = 0; j < p_treeWidget->topLevelItem(i)->childCount(); j++) {
          if(tempItems[k]->treeWidgetItem() == p_treeWidget->topLevelItem(i)->child(j)) {
            insertAt = j + totalChildCount;
            p_mosaicItems.replace(insertAt, tempItems[k]);
          }
        }
      }
      //--------------------------------------------------------------
      // After the first topLevelItem (or group) we need to keep track
      // of how many children the groups above the current group have
      // so we can insert the item at the right spot in the QList
      //--------------------------------------------------------------
      totalChildCount += p_treeWidget->topLevelItem(i)->childCount();
    }

  }


  /**
   * This slot is changes the level of detail at which mosaic
   * footprints will be allowed to have transparency.  This is
   * adjustable because it affects the speed of the re-paint
   * calls.
   *
   * @param detail
   */
  void MosaicWidget::changeLevelOfDetail(int detail) {
    for(int i = 0; i < p_mosaicItems.size(); i++) {
      p_mosaicItems[i]->setLevelOfDetail(detail * 0.005);
      p_mosaicItems[i]->update();
    }

  }


  /**
   * This method refits the items in the graphics view.
   *
   */
  void MosaicWidget::refit() {
    p_xmin = DBL_MAX;
    p_xmax = -DBL_MAX;
    p_ymin = DBL_MAX;
    p_ymax = -DBL_MAX;

    for(int i = 0; i < p_treeWidget->topLevelItemCount(); i++) {
      for(int j = 0; j < p_treeWidget->topLevelItem(i)->childCount(); j++) {
        MosaicItem *mosaicItem = p_treeToMosaicMap[p_treeWidget->topLevelItem(i)->child(j)];
        if(mosaicItem->XMinimum() < p_xmin) p_xmin = mosaicItem->XMinimum();
        if(mosaicItem->YMinimum() < p_ymin) p_ymin = mosaicItem->YMinimum();
        if(mosaicItem->XMaximum() > p_xmax) p_xmax = mosaicItem->XMaximum();
        if(mosaicItem->YMaximum() > p_ymax) p_ymax = mosaicItem->YMaximum();
      }
    }
    p_graphicsView->setSceneRect(p_xmin - 5, p_ymin - 5, (p_xmax - p_xmin), (p_ymax - p_ymin));
    p_graphicsView->fitInView(p_xmin - 5, p_ymin - 5, (p_xmax - p_xmin), (p_ymax - p_ymin), Qt::KeepAspectRatio);

  }


  /**
   * This slot is called when a mosaicItem or tree widget group
   * has been requested to send to back.  This slot figures out
   * which kind of sendToBack needs to happen, then called the
   * appropriate method.
   *
   */
  void MosaicWidget::sendToBack() {
    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    if(!mosaicItems.isEmpty()) {
      sendToBack(selectedMosaicItems().first());
    }
    else {
      sendToBack(selectedGroups().first());
    }

    //---------------------------------------------------------
    // Need to re-order the items in the p_mosaicItems list
    // so that when the project gets saved, it will be saved in
    // the correct order.
    //---------------------------------------------------------
    reorderMosaicItemsList();
  }


  /**
   * This method is called from the sendToBack slot.
   * a mosaic item has been requested to send to the back.  The
   * item is sent to the back of it's group.  fixZvalue is called
   * to change the z value of all the items in that group.
   *
   * @param item
   */
  void MosaicWidget::sendToBack(MosaicItem *item) {
    QTreeWidgetItem *group = item->treeItem()->parent();
    int index = group->indexOfChild(item->treeItem());

    // This moves the item in the tree widget.
    group->takeChild(index);
    group->insertChild(group->childCount(), item->treeItem());

    // This will reset the z-value for this item
    item->setZValue(-FLT_MAX);
    fixZValue(group);
  }


  /**
   * This method is called from the sendToBack slot. a tree widget
   * group has been requested to be sent to the back.
   * reorderAllZValues is then called to change the z value of all
   * the items in the view.
   *
   * @param group*/
  void MosaicWidget::sendToBack(QTreeWidgetItem *group) {
    int index = p_treeWidget->indexOfTopLevelItem(group);
    bool expanded = group->isExpanded();
    p_treeWidget->takeTopLevelItem(index);
    p_treeWidget->insertTopLevelItem(p_treeWidget->topLevelItemCount(), group);
    group->setExpanded(expanded);
    reorderAllZValues();
  }


  /**
   * This slot is called when the 'Move Up One' option is chosen
   * from the context menu.  This method looks at which kind of
   * item was chosen and calls the appropriate method.
   *
   */
  void MosaicWidget::moveUpOne() {
    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    if(!mosaicItems.isEmpty()) {
      moveUpOne(selectedMosaicItems().first());
    }
    else {
      moveUpOne(selectedGroups().first());
    }

  }


  /**
   * This slot is called when the 'Move Down One' option is chosen
   * from the context menu.  This method looks at which kind of
   * item was chosen and calls the appropriate method.
   *
   */
  void MosaicWidget::moveDownOne() {
    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    if(!mosaicItems.isEmpty()) {
      moveDownOne(selectedMosaicItems().first());
    }
    else {
      moveDownOne(selectedGroups().first());
    }

  }


  /**
   * Moves the selected mosaic item up.
   *
   * @param item
   */
  void MosaicWidget::moveUpOne(MosaicItem *item) {
    QTreeWidgetItem *group = item->treeItem()->parent();
    int index = group->indexOfChild(item->treeItem());
    if(index == 0) return;
    item->setZValue(-FLT_MAX);
    group->takeChild(index);
    group->insertChild(index - 1, item->treeItem());
    fixZValue(group);
    reorderMosaicItemsList();

  }


  /**
   * Moves the selected group up.
   *
   * @param group
   */
  void MosaicWidget::moveUpOne(QTreeWidgetItem *group) {
    int index = p_treeWidget->indexOfTopLevelItem(group);
    bool expanded = group->isExpanded();
    if(index == 0) return;
    p_treeWidget->takeTopLevelItem(index);
    p_treeWidget->insertTopLevelItem(index - 1, group);
    group->setExpanded(expanded);
    reorderAllZValues();
    reorderMosaicItemsList();

  }


  /**
   * Move the selected mosaic item down.
   *
   * @param item
   */
  void MosaicWidget::moveDownOne(MosaicItem *item) {
    QTreeWidgetItem *group = item->treeItem()->parent();
    int index = group->indexOfChild(item->treeItem());
    if(index == (group->childCount() - 1)) return;
    item->setZValue(-FLT_MAX);
    group->takeChild(index);
    group->insertChild(index + 1, item->treeItem());
    fixZValue(group);
    reorderMosaicItemsList();

  }


  /**
   * Moves the selected group in the Tree Widget down.
   *
   * @param group
   */
  void MosaicWidget::moveDownOne(QTreeWidgetItem *group) {
    int index = p_treeWidget->indexOfTopLevelItem(group);
    bool expanded = group->isExpanded();
    if(index == p_treeWidget->topLevelItemCount() - 1) return;
    p_treeWidget->takeTopLevelItem(index);
    p_treeWidget->insertTopLevelItem(index + 1, group);
    group->setExpanded(expanded);
    reorderAllZValues();
    reorderMosaicItemsList();
  }


  /**
  * This slot is called when a mosaicItem or tree widget group
  * has been requested to cut.  This slot figures out
  * which kind of cut needs to happen, then called the appropriate
  * method.
  *
  */
  void MosaicWidget::cut() {
    QList<MosaicItem *> items = selectedMosaicItems();
    p_pasteItems.clear();
    for(int i = 0; i < items.size(); i++) {
      QTreeWidgetItem *group = items[i]->treeItem()->parent();
      int index = group->indexOfChild(items[i]->treeItem());
      group->takeChild(index);
      p_graphicsScene->removeItem(items[i]);
      p_pasteItems.push_back(items[i]);
      p_mosaicItems.removeAt(p_mosaicItems.indexOf(items[i]));
    }

  }

  /**
  * This slot is called when a mosaicItem or tree widget group
  * has been requested to paste.  This slot figures out
  * which kind of paste needs to happen, then called the
  * appropriate method.
  *
  */
  void MosaicWidget::paste() {
    for(int i = 0; i < p_pasteItems.size(); i++) {
      QTreeWidgetItem *group = p_treeWidget->selectedItems().last()->parent();
      if(group != 0) {
        addItem(p_pasteItems[i], group->text(0));
      }
      else {
        addItem(p_pasteItems[i], p_treeWidget->selectedItems().last()->text(0));
      }
      showItem(p_pasteItems[i]);
    }
    p_pasteItems.clear();

  }


  /**
   * This paste method is called from the drop action.
   * i.e. when there is a drop of a tree widget item this paste is
   * called
   *
   * @param point
   */
  void MosaicWidget::paste(QPoint point) {
    if(p_dropItem) {
      for(int i = 0; i < p_pasteItems.size(); i++) {
        if(p_treeToMosaicMap.contains(p_dropItem)) {
          // before we call additem we need to set the insert value to something.
          int index = p_dropItem->parent()->indexOfChild(p_dropItem);
          p_dropItem->parent()->setSelected(true);
          p_insertItemAt = index + 1;
          addItem(p_pasteItems[i], p_dropItem->parent()->text(0));
          showItem(p_pasteItems[i]);
        }
        else {
          addItem(p_pasteItems[i], p_dropItem->text(0));
          showItem(p_pasteItems[i]);
        }

      }
    }

    p_pasteItems.clear();

  }


  /**
   * Allows the user to rename the group item.
   *
   */
  void MosaicWidget::renameGroup() {
    p_treeWidget->editItem(selectedGroups().first());
  }


  /**
   * This is basically only used for when a group is added or if a
   * group's name has been changed.
   *
   *
   * @param item
   * @param column
   */
  void MosaicWidget::groupChanged(QTreeWidgetItem *item, int column) {
    // if it's not a group item then return
    if(p_treeWidget->indexOfTopLevelItem(item) == -1) return;
    //-----------------------------------------
    // insert the new name into the map so that
    // all group names are accounted for.
    //----------------------------------------
    p_groupToTreeMap.insert(item->text(0), item);
  }


  /**
   * This method merges the selected groups in the Tree Widget.
   *
   */
  void MosaicWidget::mergeGroups() {
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    for(int i = 0; i < groupItems.size(); i++) {
      if(i != 0) {
        int index = p_treeWidget->indexOfTopLevelItem(groupItems[i]);
        groupItems[0]->addChildren(groupItems[i]->takeChildren());
        p_treeWidget->takeTopLevelItem(index);
      }
    }
    reorderGroupZValues(groupItems[0]);

  }


  /**
   * Changes the color of the footprint for the mosaic item.
   *
   */
  void MosaicWidget::changeColor() {
    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    if(groupItems.isEmpty() && mosaicItems.isEmpty()) return;

    QColor initialColor;
    if(!mosaicItems.isEmpty()) {
      initialColor = mosaicItems.first()->color();
    }
    else {
      initialColor = MosaicItem::randomColor();
    }

    QRgb rgb = QColorDialog::getRgba(initialColor.rgba());
    QColor color = QColor(qRed(rgb), qGreen(rgb), qBlue(rgb), qAlpha(rgb));

    for(int i = 0; i < groupItems.size(); i++) {
      for(int j = 0; j < groupItems[i]->childCount(); j++) {
        MosaicItem *mosItem = p_treeToMosaicMap[groupItems[i]->child(j)];
        mosItem->setColor(color);
      }
    }

    for(int i = 0; i < mosaicItems.size(); i++) {
      mosaicItems[i]->setColor(color);
    }
  }


  /**
   * Set the transparency of the image
   *
   */
  void MosaicWidget::changeTransparency() {
    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    if(groupItems.isEmpty() && mosaicItems.isEmpty()) return;

    int initialAlpha;
    if(!mosaicItems.isEmpty()) {
      initialAlpha = mosaicItems.first()->getImageTrans();
    }
    else {
      initialAlpha = 255;
    }

    QColor initialColor = QColor(255, 255, 0, initialAlpha);
    QRgb rgb = QColorDialog::getRgba(initialColor.rgba());
    QColor color = QColor(qRed(rgb), qGreen(rgb), qBlue(rgb), qAlpha(rgb));
    int newAlpha = color.alpha();

    for(int i = 0; i < groupItems.size(); i++) {
      for(int j = 0; j < groupItems[i]->childCount(); j++) {
        MosaicItem *mosItem = p_treeToMosaicMap[groupItems[i]->child(j)];
        mosItem->setTransparency(newAlpha);
      }
    }

    for(int i = 0; i < mosaicItems.size(); i++) {
      mosaicItems[i]->setTransparency(newAlpha);
    }
  }


  /**
   * Show the item label.
   *
   */
  void MosaicWidget::showLabel() {
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    for(int i = 0; i < groupItems.size(); i++) {
      for(int j = 0; j < groupItems[i]->childCount(); j++) {
        MosaicItem *mosItem = p_treeToMosaicMap[groupItems[i]->child(j)];
        mosItem->setLabelVisible(true);
      }
    }

    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    for(int i = 0; i < mosaicItems.size(); i++) {
      mosaicItems[i]->setLabelVisible(true);
    }

  }


  /**
   * Hide the item label.
   *
   */
  void MosaicWidget::hideLabel() {
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    for(int i = 0; i < groupItems.size(); i++) {
      for(int j = 0; j < groupItems[i]->childCount(); j++) {
        MosaicItem *mosItem = p_treeToMosaicMap[groupItems[i]->child(j)];
        mosItem->setLabelVisible(false);
      }
    }

    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    for(int i = 0; i < mosaicItems.size(); i++) {
      mosaicItems[i]->setLabelVisible(false);
    }

  }


  /**
  * Show the item outline only.
  *
  */
  void MosaicWidget::showOutline() {
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    for(int i = 0; i < groupItems.size(); i++) {
      for(int j = 0; j < groupItems[i]->childCount(); j++) {
        MosaicItem *mosItem = p_treeToMosaicMap[groupItems[i]->child(j)];
        mosItem->setOutlineVisible(true);
      }
    }

    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    for(int i = 0; i < mosaicItems.size(); i++) {
      mosaicItems[i]->setOutlineVisible(true);
    }

  }


  /**
   * Don't show the item outline.
   *
   */
  void MosaicWidget::hideOutline() {
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    for(int i = 0; i < groupItems.size(); i++) {
      for(int j = 0; j < groupItems[i]->childCount(); j++) {
        MosaicItem *mosItem = p_treeToMosaicMap[groupItems[i]->child(j)];
        mosItem->setOutlineVisible(false);
      }
    }

    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    for(int i = 0; i < mosaicItems.size(); i++) {
      mosaicItems[i]->setOutlineVisible(false);
    }

  }


  /**
   * Hides the item's footprint
   *
   */
  void MosaicWidget::hideFootprint() {
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    for(int i = 0; i < groupItems.size(); i++) {
      for(int j = 0; j < groupItems[i]->childCount(); j++) {
        MosaicItem *mosItem = p_treeToMosaicMap[groupItems[i]->child(j)];
        mosItem->setFootprintVisible(false);
      }
    }

    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    for(int i = 0; i < mosaicItems.size(); i++) {
      mosaicItems[i]->setFootprintVisible(false);
    }

  }


  /**
   * Shows the item's footprint
   *
   */
  void MosaicWidget::showFootprint() {
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    for(int i = 0; i < groupItems.size(); i++) {
      for(int j = 0; j < groupItems[i]->childCount(); j++) {
        MosaicItem *mosItem = p_treeToMosaicMap[groupItems[i]->child(j)];
        mosItem->setFootprintVisible(true);
      }
    }

    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    for(int i = 0; i < mosaicItems.size(); i++) {
      mosaicItems[i]->setFootprintVisible(true);
    }

  }


  /**
   * Bring up a font dialog box to allow the user to change the
   * font size of the item labels.
   *
   */
  void MosaicWidget::setLabelFont() {
    // This section is for groups
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    if(groupItems.size() > 0) {
      bool ok;
      const QString caption = "Qmos rules! Select your font size";
      QFont font = QFontDialog::getFont(&ok, QFont("Helvetica", 10), p_parent, caption);
      if(ok) {
        for(int i = 0; i < groupItems.size(); i++) {
          for(int j = 0; j < groupItems[i]->childCount(); j++) {
            MosaicItem *mosItem = p_treeToMosaicMap[groupItems[i]->child(j)];
            mosItem->setFontSize(font);
          }
        }
      }

    }

    // This section is for when just a mosaic item is selected.
    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    for(int i = 0; i < mosaicItems.size(); i++) {
      mosaicItems[i]->setFontSize();
    }

  }


  /**
   * Hides the footprint for all currently selected items.
   *
   */
  void MosaicWidget::hideItem() {
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    for(int i = 0; i < groupItems.size(); i++) {
      for(int j = 0; j < groupItems[i]->childCount(); j++) {
        MosaicItem *mosItem = p_treeToMosaicMap[groupItems[i]->child(j)];
        mosItem->setItemVisible(false);
      }
    }

    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    for(int i = 0; i < mosaicItems.size(); i++) {
      mosaicItems[i]->setItemVisible(false);
    }
  }


  /**
   * shows the passed in item.
   *
   * @param item
   */
  void MosaicWidget::showItem(MosaicItem *item) {
    item->setItemVisible(true);
  }


  /**
   * Sets all the currently selected items to visible.
   *
   */
  void MosaicWidget::showItem() {
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    for(int i = 0; i < groupItems.size(); i++) {
      for(int j = 0; j < groupItems[i]->childCount(); j++) {
        MosaicItem *mosItem = p_treeToMosaicMap[groupItems[i]->child(j)];
        mosItem->setItemVisible(true);
      }
    }

    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    for(int i = 0; i < mosaicItems.size(); i++) {
      mosaicItems[i]->setItemVisible(true);
    }
  }


  /**
   * Hides the currently selected items images
   *
   */
  void MosaicWidget::hideImage() {
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    for(int i = 0; i < groupItems.size(); i++) {
      for(int j = 0; j < groupItems[i]->childCount(); j++) {
        MosaicItem *mosItem = p_treeToMosaicMap[groupItems[i]->child(j)];
        mosItem->setImageVisible(false);
      }
    }

    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    for(int i = 0; i < mosaicItems.size(); i++) {
      mosaicItems[i]->setImageVisible(false);
    }
  }


  /**
   * Hides the item's image.
   *
   * @param item
   */
  void MosaicWidget::hideImage(MosaicItem *item) {
    item->setImageVisible(false);
  }


  /**
   * Shows the item's image.
   *
   * @param item
   */
  void MosaicWidget::showImage(MosaicItem *item) {
    item->setImageVisible(true);
  }


  /**
   * Sets the currently selected items images to visible.
   *
   */
  void MosaicWidget::showImage() {
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    for(int i = 0; i < groupItems.size(); i++) {
      for(int j = 0; j < groupItems[i]->childCount(); j++) {
        MosaicItem *mosItem = p_treeToMosaicMap[groupItems[i]->child(j)];
        mosItem->setImageVisible(true);
      }
    }

    QList <MosaicItem *> mosaicItems = selectedMosaicItems();
    for(int i = 0; i < mosaicItems.size(); i++) {
      mosaicItems[i]->setImageVisible(true);
    }
  }


  /**
   * Returns a list of all the selected groups in the Tree Widget.
   *
   * @return QList<QTreeWidgetItem*>
   */
  QList<QTreeWidgetItem *> MosaicWidget::selectedGroups() {
    QList<QTreeWidgetItem *> list;
    QMap<QString, QTreeWidgetItem *>::const_iterator it = p_groupToTreeMap.constBegin();
    while(it != p_groupToTreeMap.constEnd()) {
      if(it.value()->isSelected()) list.push_back(it.value());
      ++it;
    }
    return list;
  }


  /**
   * Returns a list of all the mosaic items selected in the Tree
   * Widget.
   *
   * @return QList<MosaicItem*>
   */
  QList<MosaicItem *> MosaicWidget::selectedMosaicItems() {
    QList<MosaicItem *> list;
    for(int i = 0; i < p_mosaicItems.size(); i++) {
      if(p_mosaicItems[i]->isSelected()) {
        list.push_back(p_mosaicItems[i]);
      }
      QList<QGraphicsItem *>children = p_mosaicItems[i]->children();
      for(int j = 0; j < children.size(); j++) {
        if(children[j]->isSelected()) {
          //---------------------------------------------------------------
          // If the child of an item has been selected, but the item itself
          // has not been added to the list, then we need to add it.
          //---------------------------------------------------------------
          if(!list.contains(p_mosaicItems[i])) {
            list.push_back(p_mosaicItems[i]);
          }
        }
      }
    }
    return list;
  }


  /**
   * Sets the current projection.
   *
   * @param proj
   */
  void MosaicWidget::setProjection(Isis::Projection *proj) {
    if(p_textItem != NULL) p_graphicsScene->removeItem(p_textItem);
    p_projection = proj;
  }


  /**
   *
   *
   *
   * @param domain
   */
  void MosaicWidget::setLonDomain(QString domain) {
    p_lonDomain = domain;
  }


  /**
   * Reprojects all the items in the view.
   * Also makes sure to resize the view rectangle to fit the newly
   * projected footprints.
   *
   */
  void MosaicWidget::reprojectItems() {
    for(int i = 0; i < p_mosaicItems.size(); i++) {
      p_mosaicItems[i]->reproject();
    }
    p_graphicsView->update();
    if(p_mosaicItems.size() > 0) refit();
  }


  /**
   * When the user selects a map file, we need to set the
   * projection and reproject all the items in the view.
   *
   */
  void MosaicWidget::setMapFile() {
    p_mapfile = QFileDialog::getOpenFileName((QWidget *)parent(),
                "Select file to load",
                ".",
                "All Files (*.map)");
    if(p_mapfile.isEmpty()) return;

    try {
      Isis::Pvl pvl;
      pvl.Read(p_mapfile.toStdString());
      Isis::Projection *proj = Isis::ProjectionFactory::Create(pvl);
      //TODO: how does the old projection get deleted?
      setProjection(proj);
      Isis::PvlKeyword projectionKeyword = pvl.FindKeyword("ProjectionName", Isis::Pvl::Traverse);
      Isis::PvlKeyword longDomainKeyword = pvl.FindKeyword("LongitudeDomain", Isis::Pvl::Traverse);
      QString projName = QString::fromStdString(projectionKeyword[0]);
      setLonDomain(QString::fromStdString(longDomainKeyword[0]));
      p_mapDisplay->setText(projName);
      reprojectItems();
    }
    catch(Isis::iException &e) {
      std::string msg = e.Errors();
      QMessageBox::information(this, "Error", QString::fromStdString(msg), QMessageBox::Ok);
      return;
    }
  }


  /**
   * Sets the map file (and projection) to the user specified file.
   *
   * @param mapFile
   */
  void MosaicWidget::setMapFile(QString mapfile) {
    p_mapfile = mapfile;

    try {
      Isis::Pvl pvl;
      pvl.Read(p_mapfile.toStdString());
      Isis::Projection *proj = Isis::ProjectionFactory::Create(pvl);
      //TODO: how does the old projection get deleted?
      setProjection(proj);
      Isis::PvlKeyword projectionKeyword = pvl.FindKeyword("ProjectionName", Isis::Pvl::Traverse);
      Isis::PvlKeyword longDomainKeyword = pvl.FindKeyword("LongitudeDomain", Isis::Pvl::Traverse);
      QString projName = QString::fromStdString(projectionKeyword[0]);
      setLonDomain(QString::fromStdString(longDomainKeyword[0]));
      p_mapDisplay->setText(projName);
      reprojectItems();
    }
    catch(Isis::iException &e) {
      std::string msg = e.Errors();
      QMessageBox::information(this, "Error", QString::fromStdString(msg), QMessageBox::Ok);
      return;
    }
  }


  /**
   * Saves the list of filenames in the tree widget to a text
   * file.
   *
   * @param filename
   */
  void MosaicWidget::saveList(QString filename) {
    QFile file(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    for(int i = 0; i < p_treeWidget->topLevelItemCount(); i++) {
      out << p_treeWidget->topLevelItem(i)->text(0) << "-group" << "\n";
      for(int j = 0; j < p_treeWidget->topLevelItem(i)->childCount(); j ++) {
        out << p_treeWidget->topLevelItem(i)->child(j)->text(0) << ".cub\n";
      }
    }
    file.close();
  }


  /**
   * This method reads in the user selects file which was written
   * in a pvl format by the method below.
   * A mosaic item is created, then all the characteristics of
   * the item are set to what they were saved as, then the item
   * is added to the scene, in the group that was saved.
   *
   * @param filename
   */
  void MosaicWidget::readProject(QString filename) {
    Isis::Pvl pvl;
    pvl.Read(filename.toStdString());
    if(pvl.Groups() < 1) return;
    //---------------------------------------------------------
    // If there are already items open in qmos, we need to clear
    // out everything that is open and 'clear the slate' for the
    // new project.
    //----------------------------------------------------------
    deleteAllCubes();

    //----------------------------------------------------------
    // When we read in a project, we need to delete the default
    // group added in the constructor.
    //----------------------------------------------------------
    deleteGroup("Group1");

    //----------------------------------------------------
    // if the control points are visible, then we need to
    // check the control points button.
    //----------------------------------------------------
    if(pvl.HasGroup("Control Points")) {
      Isis::PvlGroup pointsGroup = pvl.FindGroup("Control Points");
      Isis::iString visible = pointsGroup.FindKeyword("Visible")[0];
      bool checked = visible.Equal("True");
      p_controlPointButton->setChecked(checked);
    }

    // setup map file
    if(pvl.HasGroup("Map File")) {
      Isis::PvlGroup mapGroup = pvl.FindGroup("Map File");
      QString mapFile = mapGroup.FindKeyword("Filename")[0].ToQt();
      if(mapFile.compare("Null") != 0) setMapFile(mapFile);
    }

    // setup control net if necessary
    if(pvl.HasGroup("Control Net File")) {
      Isis::PvlGroup grp = pvl.FindGroup("Control Net File");
      std::string netFile = grp.FindKeyword("Filename")[0];
      p_controlnetfile = QString::fromStdString(netFile);
      if(netFile.compare("Null") != 0) p_cn = new Isis::ControlNet(netFile);
    }

    // Now create each mosaic item.
    for(int i = 0; i < pvl.Groups() - 1; i++) {
      QString item = "Item #" + QString::number(i);
      if(pvl.HasGroup(item.toStdString())) {
        Isis::PvlGroup grp = pvl.FindGroup(item.toStdString());
        QString groupName = grp.FindKeyword("Group_Name")[0].ToQt();
        QString itemFileName = grp.FindKeyword("Filename")[0].ToQt();

        //-------------------------------------------------------------------
        // Create a mosaic item for every group in the project file.
        // The item is create with the pvl group as a arg. for the constructor
        //-------------------------------------------------------------------
        MosaicItem *mosItem = new MosaicItem(itemFileName, this, &grp);
        // Add the item to the scene.
        addItem(mosItem, groupName);

        //----------------------------------------------------
        // If the control points are set to visible, then call
        // displayControlPoints() for each item being created.
        //----------------------------------------------------
        if(pvl.FindGroup("Control Points").FindKeyword("Visible")[0].compare("True")
            == 0 && p_cn != NULL) {
          mosItem->displayControlPoints(p_cn);
        }
      }
    }

  }


  /**
   * This method writes the current state of all the mosaic items
   * to a pvl file so it can be read in by the method above
   * when the user selects it.
   *
   * @param filename
   */
  void MosaicWidget::saveProject(QString filename) {
    Isis::Pvl pvl;
    Isis::PvlGroup controlGroup("Control Points");

    if(p_controlPointButton->isChecked()) {
      controlGroup += Isis::PvlKeyword("Visible", "True");
    }
    else {
      controlGroup += Isis::PvlKeyword("Visible", "False");
    }
    pvl.AddGroup(controlGroup);

    // Write out the control net file to the pvl.
    Isis::PvlGroup controlNetGroup("Control Net File");
    if(p_controlPointButton->isChecked()) {
      controlNetGroup += Isis::PvlKeyword("Filename", p_controlnetfile);
    }
    else {
      controlNetGroup += Isis::PvlKeyword("Filename", "Null");
    }
    pvl.AddGroup(controlNetGroup);

    // Write out the map file to the pvl.
    Isis::PvlGroup mapGroup("Map File");
    mapGroup += Isis::PvlKeyword("Filename", p_mapfile);
    pvl.AddGroup(mapGroup);

    //----------------------------------------------------------------
    // Loop thru all the items and have them write out their current
    // state to a pvl group, then add the group to this pvl.
    //----------------------------------------------------------------
    for(int i = 0; i < p_mosaicItems.size(); i++) {
      Isis::PvlGroup grp = p_mosaicItems[i]->saveState();
      QString item = "Item #" + QString::number(i);
      grp.SetName(item.toStdString());
      pvl.AddGroup(grp);
    }

    pvl.Write(filename.toStdString());
  }


  /**
   * This event filter is installed on both the QSplitter and the
   * QGraphicsscene.
   *
   * @param o
   * @param e
   *
   * @return bool
   */
  bool MosaicWidget::eventFilter(QObject *o, QEvent *e) {
    switch(e->type()) {
        //-----------------------------------------------------
        // This event is caught from the QSplitter to decide
        // that we need to zoom in and zoom out due to the +/-
        // keys being pressed.
        //-----------------------------------------------------
      case QEvent::KeyPress: {
          if(((QKeyEvent *)e)->key() == Qt::Key_Plus) {
            p_ztool->zoomIn2X();
          }
          else  if(((QKeyEvent *)e)->key() == Qt::Key_Minus) {
            p_ztool->zoomOut2X();
          }

          if(((QKeyEvent *)e)->key() == Qt::Key_Delete) {
            deleteCube();
          }

          break;
        }

        //-------------------------------------------------------------
        // The user had pressed the mouse button in the graphics scene.
        // If the rubberband mode is on, we need to create a rubberband
        // whos parent is the graphics view and set the geometry to the
        // point at which the user click and size = 0.
        //------------------------------------------------------------
      case QMouseEvent::GraphicsSceneMousePress: {

          //-------------------------------------------------------------------
          // Since the rubberband's parent is the graphics view, we need to map
          // the mouse coordinates from the scene coords. to the view's coords.
          //-------------------------------------------------------------------
          p_origin = p_graphicsView->mapFromScene(((QGraphicsSceneMouseEvent *)e)->scenePos());

          if(p_ztool->isActive()) {
            //----------------------------------------------------
            // This makes it so that the image doesn't
            // try the redraw the entire time the user is dragging
            // the rubberband for a zoom in/out
            //----------------------------------------------------
            for(int i = 0; i < p_mosaicItems.size(); i++) {
              p_mosaicItems[i]->setEnableRepaint(false);
            }

            if(((QGraphicsSceneMouseEvent *)e)->button() == Qt::RightButton) {
              p_graphicsView->viewport()->setCursor(QCursor(QPixmap("/usgs/cpkgs/isis3/data/base/icons/viewmag-.png")));
            }
            else if(((QGraphicsSceneMouseEvent *)e)->button() == Qt::LeftButton) {
              p_graphicsView->viewport()->setCursor(QCursor(QPixmap("/usgs/cpkgs/isis3/data/base/icons/viewmag+.png")));
            }

            if(p_rubberBand == NULL) {
              p_rubberBand = new QRubberBand(QRubberBand::Rectangle, p_graphicsView);
            }
            p_rubberBand->setGeometry(QRect(p_origin, QSize()));
            p_rubberBand->show();
          }

          //-----------------------------------------------------------------------
          // if the select tool is active, then we will allow the user to  move the
          // item's label around (by dragging).  But ONLY when the select tool
          // is active.
          //-----------------------------------------------------------------------
          for(int i = 0; i < p_mosaicItems.size(); i++) {
            p_mosaicItems[i]->getLabel()->setFlag(QGraphicsItem::ItemIsMovable, (p_stool->isActive()) ? true : false);
          }

          break;
        }

        //--------------------------------------------------------------------------
        // As the user moves the mouse around we reset the geometry of the rubberband
        // such that they upper left corner is the original point at which they
        // clicked and the lower right corner is where the mouse currently is located.
        //--------------------------------------------------------------------------
      case QMouseEvent::GraphicsSceneMouseMove: {
          // update the labels in the lower right corner of the qmos window.
          if(p_mosaicItems.size() > 0) {
            p_ttool->updateLabels(((QGraphicsSceneMouseEvent *)e)->scenePos());
          }

          if(p_ztool->isActive() && p_rubberBand != NULL) {
            QPoint newPoint = p_graphicsView->mapFromScene(((QGraphicsSceneMouseEvent *)e)->scenePos());
            p_rubberBand->setGeometry(QRect(p_origin, newPoint).normalized());
          }
          break;
        }

        //-------------------------------------------------------------------
        // Once the user releases the mouse button we need to do the work to
        // zoom in, then hide the rubberband.
        //-------------------------------------------------------------------
      case QMouseEvent::GraphicsSceneMouseRelease: {
          if(p_rubberBand != NULL) {
            // Do Zoom tool stuff if Zoom tool is active
            if(p_ztool->isActive()) {
              // re-enable paint when the mouse button is released.
              for(int i = 0; i < p_mosaicItems.size(); i++) {
                p_mosaicItems[i]->setEnableRepaint(true);
              }
              p_graphicsView->viewport()->setCursor(QCursor(Qt::ArrowCursor));
              p_ztool->rubberBandComplete(p_rubberBand->geometry(), (QGraphicsSceneMouseEvent *)e);
            }
            p_rubberBand->hide();
          }
          // Do Point tool stuff if Point tool is active
          if(p_pntool->isActive() && p_cn != NULL) {
            p_pntool->findPoint(((QGraphicsSceneMouseEvent *)e)->scenePos(), p_cn);
          }

          break;
        }

      default: {
        }
    }

    return false;
  }

  /**
   * When the user drags and drop items around in the tree widget,
   * this method handles the movement of the item.
   *
   * @param point
   */
  void MosaicWidget::dropAction(QPoint point) {
    if(p_treeWidget->itemAt(point)) {
      p_dropItem = p_treeWidget->itemAt(point);
      cut();
      paste(point);
    }
  }


  /**
   * Handles the actions from the view menu.
   *
   *
   * @param action
   */
  void MosaicWidget::viewMenuAction(QAction *action) {

    if(action->text() == "Item Column") {
      if(action->isChecked()) {
        p_treeWidget->showColumn(ItemColumn);
      }
      else {
        p_treeWidget->hideColumn(ItemColumn);
      }
    }
    if(action->text() == "Footprint Column") {
      if(action->isChecked()) {
        p_treeWidget->showColumn(FootprintColumn);
      }
      else {
        p_treeWidget->hideColumn(FootprintColumn);
      }
    }

    if(action->text() == "Outline Column") {
      if(action->isChecked()) {
        p_treeWidget->showColumn(OutlineColumn);
      }
      else {
        p_treeWidget->hideColumn(OutlineColumn);
      }
    }

    if(action->text() == "Image Column") {
      if(action->isChecked()) {
        p_treeWidget->showColumn(ImageColumn);
      }
      else {
        p_treeWidget->hideColumn(ImageColumn);
      }
    }


    if(action->text() == "Label Column") {
      if(action->isChecked()) {
        p_treeWidget->showColumn(LabelColumn);
      }
      else {
        p_treeWidget->hideColumn(LabelColumn);
      }
    }


    if(action->text() == "Resolution Column") {
      if(action->isChecked()) {
        p_treeWidget->showColumn(ResolutionColumn);
      }
      else {
        p_treeWidget->hideColumn(ResolutionColumn);
      }
    }

    if(action->text() == "Emission Angle Column") {
      if(action->isChecked()) {
        p_treeWidget->showColumn(EmissionColumn);
      }
      else {
        p_treeWidget->hideColumn(EmissionColumn);
      }
    }

    if(action->text() == "Incidence Angle Column") {
      if(action->isChecked()) {
        p_treeWidget->showColumn(IncidenceColumn);
      }
      else {
        p_treeWidget->hideColumn(IncidenceColumn);
      }
    }

    if(action->text() == "Island Column") {
      if(action->isChecked()) {
        p_treeWidget->showColumn(IslandColumn);
      }
      else {
        p_treeWidget->hideColumn(IslandColumn);
      }
    }

    if(action->text() == "Notes Column") {
      if(action->isChecked()) {
        p_treeWidget->showColumn(NotesColumn);
      }
      else {
        p_treeWidget->hideColumn(NotesColumn);
      }
    }

    if(action->text() == "Show Reference Footprint") {
      setReferenceItemVisible(action->isChecked());
    }

  }


  /**
   * Provides the user with a file open dialog box so they can select a .net
   * file which contains all the points in their control net which we need
   * to display.  Each mosaic item displays it's own control points, so we
   * loop through all the mosaic items and call it's
   * displayControlPoints function.
   *
   */
  void MosaicWidget::displayControlPoints() {
    // If the tool tip says hide, then call the hide method and return.
    if(p_controlPointButton->isChecked()) {
      hideControlPoints();
      return;
    }

    //---------------------------------------------------------------
    // if the control net file has already been read in once, then
    // we don't want to do that again, just set the points to visible
    // for each mosaic item and return.
    // This wasn't such a welcome feature.
    //---------------------------------------------------------------
    //###WAS COMMENTED OUT###
    if(p_cn != NULL) {
      for(int i = 0; i < p_mosaicItems.size(); i++) {
        p_mosaicItems[i]->setControlPointsVisible(true);
      }
      return;
    }
    //###END COMMENT###

    // Bring up a file dialog for user to select their cnet file.
    QString netFile = FileDialog::getOpenFileName(p_parent,
                      "Select Control Net. File",
                      QDir::current().dirName(),
                      "*.net");

    //--------------------------------------------------------------
    // if the file is not empty attempt to load in the control points
    // for each mosaic item
    //---------------------------------------------------------------
    if(!netFile.isEmpty()) {

      try {
        Isis::Filename controlnetfile(netFile.toStdString());
        p_controlnetfile = QString::fromStdString(controlnetfile.Expanded());
        p_cn = new Isis::ControlNet(netFile.toStdString());
      }
      catch(Isis::iException &e) {
        QString message = "Invalid control network.  \n";
        std::string errors = e.Errors();
        message += errors.c_str();
        e.Clear();
        QMessageBox::information(p_parent, "Error", message);
        QApplication::restoreOverrideCursor();
        p_controlPointButton->setChecked(false);
        return;
      }

      //--------------------------------------------------------------
      // for each mosaic item, we setup a QList<QPointF> that contains
      // the control points within that item.
      //--------------------------------------------------------------
      for(int i = 0; i < p_mosaicItems.size(); i++) {
        p_mosaicItems[i]->displayControlPoints(p_cn);
      }


    }
    else {
      //---------------------------------------------------
      // this means the user canceled out of the dialog box
      //---------------------------------------------------
      p_controlPointButton->setChecked(false);
    }

    //------------------------------------------------------
    // Set the tool tip text to what will happen if the user
    // presses the button again.
    //-----------------------------------------------------
    p_controlPointButton->setToolTip("Hide Control Points");
  }


  /**
   * Displays the connectivity of Control Points
   *
   */
  void MosaicWidget::displayConnectivity(bool connected) {
    if(p_cn == NULL) {
      //p_connectivityButton->setChecked(true);
      return;
    }
    //if(p_connectivityButton->isChecked() ) { //###Can the colors be reverted to the same as on load?###
    if(!connected) {   //###Can the colors be reverted to the same as on load?###
      for(int i = 0; i < p_mosaicItems.size(); i++) {
        p_mosaicItems[i]->setColor(MosaicItem::randomColor());
      }
      //p_connectivityButton->setToolTip("Display Network Connectivity");
      return;
    }

    // Color and label islands
    Isis::ControlGraph *graph = new Isis::ControlGraph(p_cn);

    if(graph->GetIslandCount() == 0) {
      QColor islandColor = MosaicItem::randomColor();
      for(int cube = 0; cube < p_mosaicItems.size(); cube ++) {
        p_mosaicItems[cube]->treeItem()->setText(IslandColumn, QString::number(1));
        p_mosaicItems[cube]->setColor(islandColor);
      }
    }
    else {
      for(int island = 0; island < graph->GetIslandCount(); island ++) {
        QColor islandColor = MosaicItem::randomColor();

        const QVector< QString > snList = graph->GetCubesOnIsland(island);

        for(int sn = 0; sn < snList.size(); sn ++) {
          for(int cube = 0; cube < p_mosaicItems.size(); cube ++) {
            if(snList[sn] == p_mosaicItems[cube]->serialNumber().c_str()) {
              p_mosaicItems[cube]->treeItem()->setText(IslandColumn, QString::number(island + 1));
              p_mosaicItems[cube]->setColor(islandColor);
            }
          }
        }
      }
    }
    delete graph;
    graph = NULL;

    // Sort on Islands
    //sortByIsland(); //###This should be removed and placed into a sort button###

    p_connectivityButton->setToolTip("Reset Colors");
  }



  void MosaicWidget::setControlNet(Isis::Filename cnet) {
    p_controlnetfile = QString::fromStdString(cnet.Expanded());
    p_cn = new Isis::ControlNet(cnet.Expanded());
  }


  /**
   * Calls setControlPointsVisible(false) for each mosaic item and
   * change the control point button back to "Display".
   *
   */
  void MosaicWidget::hideControlPoints() {
    for(int i = 0; i < p_mosaicItems.size(); i++) {
      p_mosaicItems[i]->setControlPointsVisible(false);
    }
    p_controlPointButton->setToolTip("Display Control Points");
  }


  /**
   * Returns a list of all the mosaic items in the scene including
   * all of the children
   *
   *
   * @return QList<MosaicItem*>
   */
  QList<MosaicItem *> MosaicWidget::allMosaicItems() {
    QList<MosaicItem *> allItems;
    for(int i = 0; i < p_mosaicItems.size(); i++) {
      allItems.push_back(p_mosaicItems[i]);
      QList<QGraphicsItem *>children = p_mosaicItems[i]->children();
      for(int j = 0; j < children.size(); j++) {
        allItems.push_back((MosaicItem *)children[j]);
      }
    }
    return allItems;
  }


  /**
   * The next three methods are private slots called from the
   * context menu.  This allows the user to sort the items in a
   * group base on resolution, emission angle, or incidence angle.
   *
   */
  void MosaicWidget::sortByResolution() {
    sortBy(ResolutionColumn);
  }
  void MosaicWidget::sortByEmission() {
    sortBy(EmissionColumn);
  }
  void MosaicWidget::sortByIncidence() {
    sortBy(IncidenceColumn);
  }
  void MosaicWidget::sortByIsland() {
    sortBy(IslandColumn);
  }


  /**
   * This method loops through all the items in the selected
   * group(s) and sorts them based on the requested sort type.
   *
   */
  void MosaicWidget::sortBy(ColumnIndex index) {
    //------------------------------------------
    // Get make a QList of the selected group(s).
    //------------------------------------------
    QList <QTreeWidgetItem *> groupItems = selectedGroups();
    if(groupItems.isEmpty()) return;

    for(int i = 0; i < groupItems.size(); i++) {
      QList<QTreeWidgetItem *>children = groupItems[i]->takeChildren();

      if(index == ResolutionColumn) {
        std::sort(children.begin(), children.end(), sortResolution);
      }
      if(index == EmissionColumn) {
        std::sort(children.begin(), children.end(), sortEmission);
      }
      if(index == IncidenceColumn) {
        std::sort(children.begin(), children.end(), sortIncidence);
      }
      if(index == IslandColumn) {
        std::sort(children.begin(), children.end(), sortIsland);
      }

      groupItems[i]->insertChildren(0, children);
      //---------------------------
      // Get the Z-ordering correct.
      //---------------------------
      reorderGroupZValues(groupItems[i]);
    }
    //---------------------------------------------------
    // Get the p_mosaicItems list in the right order too.
    //---------------------------------------------------
    reorderMosaicItemsList();
  }


  /**
   *
   *
   *
   * @param resolution
   */
  void MosaicWidget::updateScreenResolution(double resolution) {
    p_screenResolution = resolution;
  }


  /**
   * This method is called from the context menu.
   * It allows the user to select which item they would like
   * to zoom to, and that item is then fit in the view.
   *
   */
  void MosaicWidget::zoomToItem() {
    MosaicItem *mosaicItem = selectedMosaicItems().last();
    double ymin = mosaicItem->YMinimum();
    double xmin = mosaicItem->XMinimum();
    double xmax = mosaicItem->XMaximum();
    double ymax = mosaicItem->YMaximum();
    p_graphicsView->fitInView(xmin - 5, ymin - 5, (xmax - xmin), (ymax - ymin), Qt::KeepAspectRatio);
  }


  /**
   * This is called from the std::sort method if the requested
   * sort is a resolution sort.
   *
   *
   * @param a
   * @param b
   *
   * @return bool
   */
  bool sortResolution(QTreeWidgetItem *a, QTreeWidgetItem *b) {
    return a->text(MosaicWidget::ResolutionColumn).toDouble() < b->text(MosaicWidget::ResolutionColumn).toDouble();
  }


  /**
   * This is called from the std::sort method if the requested
   * sort is an emission angle sort.
   *
   *
   * @param a
   * @param b
   *
   * @return bool
   */
  bool sortEmission(QTreeWidgetItem *a, QTreeWidgetItem *b) {
    return a->text(MosaicWidget::EmissionColumn).toDouble() < b->text(MosaicWidget::EmissionColumn).toDouble();
  }

  /**
   * This is called from the std::sort method if the requested
   * sort is an incidence sort.
   *
   *
   * @param a
   * @param b
   *
   * @return bool
   */
  bool sortIncidence(QTreeWidgetItem *a, QTreeWidgetItem *b) {
    return a->text(MosaicWidget::IncidenceColumn).toDouble() < b->text(MosaicWidget::IncidenceColumn).toDouble();
  }

  /**
   * This is called from the std::sort method if the requested sort is an island
   * sort
   *
   * @param a
   * @param b
   *
   * @return bool
   */
  bool sortIsland(QTreeWidgetItem *a, QTreeWidgetItem *b) {
    return a->text(MosaicWidget::IslandColumn).toInt() < b->text(MosaicWidget::IslandColumn).toInt();
  }

}
