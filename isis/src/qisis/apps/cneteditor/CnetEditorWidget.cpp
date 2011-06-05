#include "IsisDebug.h"

#include "CnetEditorWidget.h"

#include <iostream>

#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QByteArray>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelection>
#include <QModelIndex>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QSplitter>
#include <QString>
#include <QStringList>
#include <QTableView>
#include <QTimer>
#include <QTreeView>
#include <QVBoxLayout>

#include "ControlNet.h"
#include "ControlPoint.h"

#include "ConnectionModel.h"
#include "FilterWidget.h"
#include "MeasureTableDelegate.h"
#include "MeasureTableModel.h"
#include "PointTableDelegate.h"
#include "PointTableModel.h"
#include "PointModel.h"
#include "SerialModel.h"
#include "AbstractTreeItem.h"


using std::cerr;


namespace Isis
{

  CnetEditorWidget::CnetEditorWidget(Isis::ControlNet * cNet)
  {
    nullify();
    

//**************************************************************
//**************************************************************
//**************************************************************    
    
    VERSION = new QString("0.1");
    
//**************************************************************
//**************************************************************
//**************************************************************
    
    
    
    workingVersion = new QString;
    
    updatingSelection = false;
    controlNet = cNet;
    connect(cNet, SIGNAL(networkStructureModified()),
        this, SLOT(rebuildModels()));

    QBoxLayout * mainLayout = createMainLayout();
    setLayout(mainLayout);

    topSplitterDefault = new QByteArray(topSplitter->saveState());

    readSettings();
    
    upgradeVersion();
  }


  CnetEditorWidget::~CnetEditorWidget()
  {
    writeSettings();
  }


  void CnetEditorWidget::nullify()
  {
    pointView = NULL;
    serialView = NULL;
    connectionView = NULL;

    pointModel = NULL;
    serialModel = NULL;
    connectionModel = NULL;

    editPointModel = NULL;
    editMeasureModel = NULL;

    editPointDelegate = NULL;
    editMeasureDelegate = NULL;

    editPointView = NULL;
    editMeasureView = NULL;

    topSplitter = NULL;
    mainSplitter = NULL;

    filterArea = NULL;

    pointFilterWidget = NULL;
    serialFilterWidget = NULL;
    connectionFilterWidget = NULL;

    controlNet = NULL;
    topSplitterDefault = NULL;
    workingVersion = NULL;
    VERSION = NULL;
  }


  void CnetEditorWidget::setDriverView(int driverView)
  {
    View view = (View) driverView;
    pointModel->setDrivable(view == PointView);
    serialModel->setDrivable(view == SerialView);
    connectionModel->setDrivable(view == ConnectionView);

    pointView->selectionModel()->clear();
    pointView->collapseAll();

    serialView->selectionModel()->clear();
    serialView->collapseAll();

    connectionView->selectionModel()->clear();
    connectionView->collapseAll();

    syncFilterWidgets();
  }


  void CnetEditorWidget::syncFilterWidgets()
  {
    pointFilterWidget->setVisible(false);
    serialFilterWidget->setVisible(false);
    connectionFilterWidget->setVisible(false);

    switch ((View) getDriverView())
    {
      case PointView:
        pointFilterWidget->show();
        break;
      case SerialView:
        serialFilterWidget->show();
        break;
      case ConnectionView:
        connectionFilterWidget->show();
    }

//     topSplitter->addWidget(pointFilterArea);
//     topSplitter->addWidget(serialFilterArea);
//     topSplitter->addWidget(connectionFilterArea);

  }


  int CnetEditorWidget::getDriverView() const
  {
    View driverView;
    if (pointModel->isDrivable())
    {
      driverView = PointView;
      ASSERT(!serialModel->isDrivable());
      ASSERT(!connectionModel->isDrivable());
    }
    else
    {
      if (serialModel->isDrivable())
      {
        driverView = SerialView;
        ASSERT(!pointModel->isDrivable());
        ASSERT(!connectionModel->isDrivable());
      }
      else
      {
        driverView = ConnectionView;
        ASSERT(connectionModel->isDrivable());
        ASSERT(!pointModel->isDrivable());
        ASSERT(!serialModel->isDrivable());
      }
    }
    return (int) driverView;
  }


  QBoxLayout * CnetEditorWidget::createMainLayout()
  {
    createPointView();
    createSerialView();
    createConnectionView();
    createFilterArea();

    createEditPointView();
    QGroupBox * editPointBox = new QGroupBox(tr("Control Point Table"));
    QHBoxLayout * editPointLayout = new QHBoxLayout;
    editPointLayout->addWidget(editPointView);
    editPointBox->setLayout(editPointLayout);

    createEditMeasureView();
    QGroupBox * editMeasureBox = new QGroupBox(tr("Control Measure Table"));
    QHBoxLayout * editMeasureLayout = new QHBoxLayout;
    editMeasureLayout->addWidget(editMeasureView);
    editMeasureBox->setLayout(editMeasureLayout);

    topSplitter = new QSplitter(Qt::Horizontal);
//     topSplitter->setChildrenCollapsible(false);
    topSplitter->addWidget(pointView);
    topSplitter->addWidget(serialView);
    topSplitter->addWidget(connectionView);
    topSplitter->addWidget(filterArea);

    mainSplitter = new QSplitter(Qt::Vertical);
    mainSplitter->addWidget(topSplitter);
    mainSplitter->addWidget(editPointBox);
    mainSplitter->addWidget(editMeasureBox);

    QBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->addWidget(mainSplitter);

    return mainLayout;
  }


  void CnetEditorWidget::createPointView()
  {
    pointView = new QTreeView();
    pointModel = new PointModel(controlNet, "Point View", pointView, qApp);
    pointView->setModel(pointModel);
    connect(pointView->selectionModel(), SIGNAL(selectionChanged(
        const QItemSelection &, const QItemSelection &)), this,
        SLOT(pointViewSelectionChanged(const QItemSelection &,
        const QItemSelection &)));
    connect(pointView, SIGNAL(expanded(const QModelIndex &)), this,
        SLOT(itemExpanded(const QModelIndex &)));
    connect(pointView, SIGNAL(collapsed(const QModelIndex &)), this,
        SLOT(itemCollapsed(const QModelIndex &)));
    pointView->setAlternatingRowColors(true);
    pointView->setSelectionMode(QAbstractItemView::MultiSelection);
  }


  void CnetEditorWidget::createSerialView()
  {
    serialView = new QTreeView();
    serialModel = new SerialModel(controlNet, "Cube View", serialView, qApp);
    serialView->setModel(serialModel);
    connect(serialView->selectionModel(), SIGNAL(selectionChanged(
        const QItemSelection &, const QItemSelection &)), this,
        SLOT(serialViewSelectionChanged(const QItemSelection &,
        const QItemSelection &)));
    connect(serialView, SIGNAL(expanded(const QModelIndex &)), this,
        SLOT(itemExpanded(const QModelIndex &)));
    connect(serialView, SIGNAL(collapsed(const QModelIndex &)), this,
        SLOT(itemCollapsed(const QModelIndex &)));
    serialView->setAlternatingRowColors(true);
    serialView->setSelectionMode(QAbstractItemView::MultiSelection);
  }


  void CnetEditorWidget::createConnectionView()
  {
    connectionView = new QTreeView();
    connectionModel = new ConnectionModel(controlNet, "Cube Connection View",
        connectionView, qApp);
    connectionView->setModel(connectionModel);
    connect(connectionView->selectionModel(), SIGNAL(selectionChanged(
        const QItemSelection &, const QItemSelection &)), this,
        SLOT(connectionViewSelectionChanged(const QItemSelection &,
            const QItemSelection &)));
    connect(connectionView, SIGNAL(expanded(const QModelIndex &)), this,
        SLOT(itemExpanded(const QModelIndex &)));
    connect(connectionView, SIGNAL(collapsed(const QModelIndex &)), this,
        SLOT(itemCollapsed(const QModelIndex &)));
    connectionView->setAlternatingRowColors(true);
    connectionView->setSelectionMode(QAbstractItemView::MultiSelection);
  }


  void CnetEditorWidget::createFilterArea()
  {
    ASSERT(pointModel);
    ASSERT(serialModel);
    ASSERT(connectionModel);
    
    pointFilterWidget = new FilterWidget("Points and Measures");
    connect(pointFilterWidget, SIGNAL(filterChanged()),
        this, SLOT(rebuildModels()));
    if (pointModel)
      pointModel->setFilter(pointFilterWidget);
      
    serialFilterWidget = new FilterWidget("Images and Points");
    connect(serialFilterWidget, SIGNAL(filterChanged()),
        this, SLOT(rebuildModels()));
    if (serialModel)
      serialModel->setFilter(serialFilterWidget);
      
    connectionFilterWidget = new FilterWidget("Connections");
    connect(connectionFilterWidget, SIGNAL(filterChanged()),
        this, SLOT(rebuildModels()));
    if (connectionModel)
      connectionModel->setFilter(connectionFilterWidget);

    QHBoxLayout * layout = new QHBoxLayout;
    layout->addWidget(pointFilterWidget);
    layout->addWidget(serialFilterWidget);
    layout->addWidget(connectionFilterWidget);

    QWidget * filterAreaWidget = new QWidget;
    filterAreaWidget->setLayout(layout);
    connect(pointFilterWidget, SIGNAL(scrollToBottom()),
        this, SLOT(scrollFilterAreaToBottom()));
    connect(serialFilterWidget, SIGNAL(scrollToBottom()),
        this, SLOT(scrollFilterAreaToBottom()));
    connect(connectionFilterWidget, SIGNAL(scrollToBottom()),
        this, SLOT(scrollFilterAreaToBottom()));

    filterArea = new QScrollArea;
    filterArea->setWidget(filterAreaWidget);
    filterArea->setWidgetResizable(true);
  }


  void CnetEditorWidget::createEditPointView()
  {
    editPointView = new QTableView();
    editPointModel = new PointTableModel(qApp);
    editPointView->setModel(editPointModel);
    connect(editPointModel, SIGNAL(dataChanged(const QModelIndex &,
        const QModelIndex &)), editPointView, SLOT(resizeColumnsToContents()));
    editPointDelegate = new PointTableDelegate(editPointModel, editPointView);
    connect(editPointDelegate, SIGNAL(dataEdited()),
        this, SIGNAL(cnetModified()));
    editPointView->setItemDelegate(editPointDelegate);
    editPointView->resizeColumnsToContents();
    editPointView->setEditTriggers(QAbstractItemView::SelectedClicked |
        QAbstractItemView::DoubleClicked | QAbstractItemView::AnyKeyPressed);
    editPointView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  }


  void CnetEditorWidget::createEditMeasureView()
  {
    editMeasureView = new QTableView();
    editMeasureModel = new MeasureTableModel(qApp);
    editMeasureView->setModel(editMeasureModel);
    connect(editMeasureModel, SIGNAL(dataChanged(const QModelIndex &,
        const QModelIndex &)),
        editMeasureView, SLOT(resizeColumnsToContents()));
    editMeasureDelegate = new MeasureTableDelegate(editMeasureModel,
        editMeasureView);
    connect(editMeasureDelegate, SIGNAL(dataEdited()),
        this, SIGNAL(cnetModified()));
    editMeasureView->setItemDelegate(editMeasureDelegate);
    editMeasureView->resizeColumnsToContents();
    editMeasureView->setEditTriggers(QAbstractItemView::SelectedClicked |
        QAbstractItemView::DoubleClicked | QAbstractItemView::AnyKeyPressed);
    editMeasureView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  }


  void CnetEditorWidget::pointViewSelectionChanged(
    const QItemSelection & newSelected, const QItemSelection & newDeselected)
  {
    if (updatingSelection)
      return;
    updatingSelection = true;

    updateTreeItemsWithNewSelection(newSelected, newDeselected);

    QList< ControlPoint * > points;
    QList< ControlMeasure * > measures;
    QStringList cubeSerialNumbers;
    QList< QModelIndex > indexes =
      pointView->selectionModel()->selectedIndexes();
    for (int i = 0; i < indexes.size(); i++)
    {
      AbstractTreeItem * item = static_cast< AbstractTreeItem * >(
          indexes[i].internalPointer());
      AbstractTreeItem::InternalPointerType type = item->pointerType();

      if (type == AbstractTreeItem::Point)
      {
        points << controlNet->GetPoint(item->data().toString());
      }
      else
      {
        if (type == AbstractTreeItem::Measure)
        {
          QString pointId = item->parent()->data().toString();
          QString serial = item->data().toString();

          measures << controlNet->GetPoint(pointId)->GetMeasure(serial);
          cubeSerialNumbers << serial;
        }
        else
        {
          ASSERT(0);
        }
      }
    }

    // populate editor tables
    editPointModel->setPoints(points);
    QList< ControlMeasure * > allMeasuresForSelectedPoints;
    foreach(ControlPoint * point, points)
    {
      allMeasuresForSelectedPoints.append(point->getMeasures());
    }
    editMeasureModel->setMeasures(allMeasuresForSelectedPoints);

    if (pointModel->isDrivable())
    {
      focusView(serialView, cubeSerialNumbers);
      focusView(connectionView, cubeSerialNumbers);
    }

    updatingSelection = false;
  }


  void CnetEditorWidget::serialViewSelectionChanged(
    const QItemSelection & newSelected, const QItemSelection & newDeselected)
  {
    if (updatingSelection)
      return;
    updatingSelection = true;

    updateTreeItemsWithNewSelection(newSelected, newDeselected);

    QList< ControlPoint * > points;
    QList< ControlMeasure * > measures;
    QStringList pointIds;
    QStringList cubeSerialNumbers;
    QList< QModelIndex > indexes =
      serialView->selectionModel()->selectedIndexes();
    for (int i = 0; i < indexes.size(); i++)
    {
      AbstractTreeItem * item = static_cast< AbstractTreeItem * >(
          indexes[i].internalPointer());
      AbstractTreeItem::InternalPointerType type = item->pointerType();

      if (type == AbstractTreeItem::CubeGraphNode)
      {
        cubeSerialNumbers << item->data().toString();
      }
      else
      {
        if (type == AbstractTreeItem::Point)
        {
          QString pointId = item->data().toString();
          pointIds << pointId;

          ControlPoint * point = controlNet->GetPoint(pointId);
          points << point;
          measures << point->GetMeasure(item->parent()->data().toString());
        }
        else
        {
          ASSERT(0);
        }
      }
    }

    editPointModel->setPoints(points);
    editMeasureModel->setMeasures(measures);

    if (serialModel->isDrivable())
    {
      focusView(pointView, pointIds);
      focusView(connectionView, cubeSerialNumbers);
    }

    updatingSelection = false;
  }



  void CnetEditorWidget::connectionViewSelectionChanged(
    const QItemSelection & newSelected, const QItemSelection & newDeselected)
  {
    if (updatingSelection)
      return;
    updatingSelection = true;

    updateTreeItemsWithNewSelection(newSelected, newDeselected);

    QList< ControlPoint * > points;
    QList< ControlMeasure * > measures;
    QStringList pointIds;
    QStringList cubeSerialNumbers;
    QList< QModelIndex > indexes =
      connectionView->selectionModel()->selectedIndexes();
    for (int i = 0; i < indexes.size(); i++)
    {
      AbstractTreeItem * item = static_cast< AbstractTreeItem * >(
          indexes[i].internalPointer());
      AbstractTreeItem::InternalPointerType type = item->pointerType();

      if (type == AbstractTreeItem::CubeGraphNode)
      {
        cubeSerialNumbers << item->data().toString();
      }
      else
      {
        if (type == AbstractTreeItem::Point)
        {
          QString pointId = item->data().toString();
          pointIds << pointId;

          ControlPoint * point = controlNet->GetPoint(pointId);
          points << point;
//           measures << point->GetMeasure(item->parent()->data().toString()) <<
//               point->GetMeasure(item->parent()->parent()->data().toString());
        }
        else
        {
          ASSERT(0);
        }
      }
    }

    editPointModel->setPoints(points);
    editMeasureModel->setMeasures(measures);

    if (connectionModel->isDrivable())
    {
      focusView(pointView, pointIds);
      focusView(serialView, cubeSerialNumbers);
    }

    updatingSelection = false;
  }


  void CnetEditorWidget::itemExpanded(const QModelIndex & index)
  {
    static_cast< AbstractTreeItem * >(
      index.internalPointer())->setExpanded(true);
  }


  void CnetEditorWidget::itemCollapsed(const QModelIndex & index)
  {
    static_cast< AbstractTreeItem * >(
      index.internalPointer())->setExpanded(false);
  }


  void CnetEditorWidget::rebuildModels()
  {
//     cerr << "CnetEditorWidget::rebuildModels called\n";
    ASSERT(pointModel);
    ASSERT(serialModel);
    ASSERT(connectionModel);

    updatingSelection = true;

//     cerr << "CnetEditorWidget::rebuildModels one\n";
//     pointModel->saveViewState();
    pointModel->rebuildItems();
//     pointModel->loadViewState();

//     cerr << "CnetEditorWidget::rebuildModels two\n";
//     serialModel->saveViewState();
    serialModel->rebuildItems();
//     serialModel->loadViewState();

//     cerr << "CnetEditorWidget::rebuildModels three\n";
//     connectionModel->saveViewState();
    connectionModel->rebuildItems();
//     connectionModel->loadViewState();

    updatingSelection = false;
//     cerr << "CnetEditorWidget::rebuildModels done\n";
  }
  
  
  void CnetEditorWidget::scrollFilterAreaToBottom()
  {
    QTimer::singleShot(100, this, SLOT(doScroll()));
  }
  
  
  void CnetEditorWidget::doScroll()
  {
    ASSERT(filterArea);
    
    if (filterArea)
    {
      QScrollBar * vsb = filterArea->verticalScrollBar();
      vsb->setValue(vsb->maximum());
    }
  }


  void CnetEditorWidget::focusView(QTreeView * view, QStringList labels)
  {
    QAbstractItemModel * model = view->model();
    for (int i = 0; i < model->rowCount(); i++)
    {
      QModelIndex index = model->index(i, 0, QModelIndex());
      AbstractTreeItem * item = static_cast< AbstractTreeItem * >(
          index.internalPointer());

      bool hit = labels.contains(
          model->data(index, Qt::DisplayRole).toString());
      bool shouldExpand = hit && labels.size() == 1;

      item->setSelected(hit);
      view->selectionModel()->select(index, hit ? QItemSelectionModel::Select :
          QItemSelectionModel::Deselect);

      item->setExpanded(shouldExpand);
      view->setExpanded(index, shouldExpand);

      if (hit)
        view->scrollTo(index, QAbstractItemView::PositionAtCenter);
    }
  }


  void CnetEditorWidget::updateTreeItemsWithNewSelection(
    const QItemSelection & newSelected, const QItemSelection & newDeselected)
  {
    QList< QModelIndex > newSelectedIndexes = newSelected.indexes();
    for (int i = 0; i < newSelectedIndexes.size(); i++)
    {
      static_cast< AbstractTreeItem * >(
        newSelectedIndexes[i].internalPointer())->setSelected(true);
    }

    QList< QModelIndex > newDeselectedIndexes = newDeselected.indexes();
    for (int i = 0; i < newDeselectedIndexes.size(); i++)
    {
      static_cast< AbstractTreeItem * >(
        newDeselectedIndexes[i].internalPointer())->setSelected(false);
    }
  }
  
  
  void CnetEditorWidget::upgradeVersion()
  {
    if (*workingVersion == "")
    {
      topSplitter->restoreState(*topSplitterDefault);
      *workingVersion = "0.1";
    }
    
    if (*workingVersion != *VERSION)
      upgradeVersion();
  }
  

  void CnetEditorWidget::readSettings()
  {
    ASSERT(topSplitter);
    ASSERT(mainSplitter);
    ASSERT(workingVersion);
    
    QSettings settings("USGS", "CnetEditorWidget");
    *workingVersion = settings.value("version", "").toString();
    setDriverView(settings.value("driverView", PointView).toInt());
    
    topSplitter->restoreState(settings.value("topSplitter").toByteArray());
    mainSplitter->restoreState(settings.value("mainSplitter").toByteArray());
  }


  void CnetEditorWidget::writeSettings()
  {
    ASSERT(topSplitter);
    ASSERT(mainSplitter);

    QSettings settings("USGS", "CnetEditorWidget");
    settings.setValue("version", *VERSION);
    settings.setValue("topSplitter", topSplitter->saveState());
    settings.setValue("mainSplitter", mainSplitter->saveState());
    settings.setValue("driverView", getDriverView());
  }
}
