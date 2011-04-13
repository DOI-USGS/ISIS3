#include "IsisDebug.h"

#include "CnetEditorWidget.h"

#include <iostream>

#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelection>
#include <QModelIndex>
#include <QSettings>
#include <QSplitter>
#include <QString>
#include <QStringList>
#include <QTableView>
#include <QTreeView>
#include <QVBoxLayout>

#include "ControlNet.h"
#include "ControlPoint.h"

#include "ConnectionModel.h"
#include "MeasureTableDelegate.h"
#include "MeasureTableModel.h"
#include "PointTableDelegate.h"
#include "PointTableModel.h"
#include "PointModel.h"
#include "SerialModel.h"
#include "AbstractTreeItem.h"

#include <QPushButton>


using std::cerr;


namespace Isis
{

  CnetEditorWidget::CnetEditorWidget(Isis::ControlNet * cNet)
  {
    nullify();
    updatingSelection = false;
    controlNet = cNet;
    connect(cNet, SIGNAL(networkStructureModified()),
        this, SLOT(rebuildModels()));

    QBoxLayout * mainLayout = createMainLayout();
    setLayout(mainLayout);
    readSettings();
  }


  CnetEditorWidget::~CnetEditorWidget()
  {
    writeSettings();
  }


  void CnetEditorWidget::setDriverView(int driverView)
  {
    View view = (View) driverView;
    pointModel->setDrivable(view == PointView);
    serialModel->setDrivable(view == SerialView);
    connectionModel->setDrivable(view == ConnectionView);

    QTreeView * treeView = NULL;
    switch (view)
    {
      case PointView: treeView = pointView; break;
      case SerialView: treeView = serialView; break;
      case ConnectionView: treeView = connectionView; break;
    }

    treeView->selectionModel()->clear();
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
    QGroupBox * editPointBox = new QGroupBox(tr("Control Point Table"));
    QHBoxLayout * editPointLayout = new QHBoxLayout;
    editPointLayout->addWidget(editPointView);
    editPointBox->setLayout(editPointLayout);

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
    QGroupBox * editMeasureBox = new QGroupBox(tr("Control Measure Table"));
    QHBoxLayout * editMeasureLayout = new QHBoxLayout;
    editMeasureLayout->addWidget(editMeasureView);
    editMeasureBox->setLayout(editMeasureLayout);


    topSplitter = new QSplitter(Qt::Horizontal);
    topSplitter->addWidget(pointView);
    topSplitter->addWidget(serialView);
    topSplitter->addWidget(connectionView);

    mainSplitter = new QSplitter(Qt::Vertical);
    mainSplitter->addWidget(topSplitter);
    mainSplitter->addWidget(editPointBox);
    mainSplitter->addWidget(editMeasureBox);

    QBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->addWidget(mainSplitter);

    return mainLayout;
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

    controlNet = NULL;
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
    allMeasuresForSelectedPoints.append(point->GetMeasures());
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
    static_cast< AbstractTreeItem * >(index.internalPointer())->setExpanded(
      true);
  }


  void CnetEditorWidget::itemCollapsed(const QModelIndex & index)
  {
    static_cast< AbstractTreeItem * >(index.internalPointer())->setExpanded(
      false);
  }


  void CnetEditorWidget::rebuildModels()
  {
//     cerr << "CnetEditorWidget::rebuildModels called\n";
    ASSERT(pointModel);
    ASSERT(serialModel);
    ASSERT(connectionModel);

    updatingSelection = true;
    
//     cerr << "CnetEditorWidget::rebuildModels one\n";
    pointModel->saveViewState();
    pointModel->rebuildItems();
    pointModel->loadViewState();

//     cerr << "CnetEditorWidget::rebuildModels two\n";
    serialModel->saveViewState();
    serialModel->rebuildItems();
    serialModel->loadViewState();

//     cerr << "CnetEditorWidget::rebuildModels three\n";
    connectionModel->saveViewState();
    connectionModel->rebuildItems();
    connectionModel->loadViewState();
    
    updatingSelection = false;
//     cerr << "CnetEditorWidget::rebuildModels done\n";
  }


  void CnetEditorWidget::handleButtonClicked()
  {
    serialModel->loadViewState();
  }


  void CnetEditorWidget::focusView(QTreeView * view, QStringList labels)
  {
    QAbstractItemModel * model = view->model();
    view->selectionModel()->clear();
    view->collapseAll();
    for (int i = 0; i < model->rowCount(); i++)
    {
      QModelIndex index = model->index(i, 0, QModelIndex());
      if (labels.contains(model->data(index, Qt::DisplayRole).toString()))
      {
        view->selectionModel()->select(index, QItemSelectionModel::Select);
        if (labels.size() == 1)
        {
          view->setExpanded(index, true);
//           cerr << "CnetEditorWidget::focusView... "
//                << static_cast< AbstractTreeItem * >(index.internalPointer())->isExpanded()
//                << "\t" << index.internalPointer() << "\n";

        }
        view->scrollTo(index, QAbstractItemView::PositionAtCenter);
      }
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


  void CnetEditorWidget::readSettings()
  {
    ASSERT(topSplitter);
    ASSERT(mainSplitter);

    QSettings settings("USGS", "CnetEditorWidget");
    topSplitter->restoreState(settings.value("topSplitter").toByteArray());
    mainSplitter->restoreState(settings.value("mainSplitter").toByteArray());
    setDriverView(settings.value("driverView", PointView).toInt());
  }


  void CnetEditorWidget::writeSettings()
  {
    ASSERT(topSplitter);
    ASSERT(mainSplitter);

    QSettings settings("USGS", "CnetEditorWidget");
    settings.setValue("topSplitter", topSplitter->saveState());
    settings.setValue("mainSplitter", mainSplitter->saveState());
    settings.setValue("driverView", getDriverView());
  }
}
