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
#include <QMenu>
#include <QModelIndex>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QSplitter>
#include <QString>
#include <QStringList>
#include <QTableView>
#include <QTime>
#include <QTimer>
#include <QVBoxLayout>

#include "ControlNet.h"
#include "ControlPoint.h"

#include "AbstractTreeItem.h"
#include "CnetView.h"
#include "ConnectionModel.h"
#include "FilterWidget.h"
#include "MeasureTableDelegate.h"
#include "MeasureTableModel.h"
#include "PointTableDelegate.h"
#include "PointTableModel.h"
#include "PointModel.h"
#include "SerialModel.h"


using std::cerr;


namespace Isis
{

//**************************************************************
//**************************************************************
//**************************************************************

  const QString CnetEditorWidget::VERSION = "0.1";

//**************************************************************
//**************************************************************
//**************************************************************


  CnetEditorWidget::CnetEditorWidget(Isis::ControlNet * cNet,
      QString pathForSettings)
  {
    nullify();

    workingVersion = new QString;

    updatingSelection = false;

    controlNet = cNet;
    connect(controlNet, SIGNAL(networkStructureModified()),
        this, SLOT(rebuildModels()));

    settingsPath = new QString(pathForSettings);

    QBoxLayout * mainLayout = createMainLayout();
    setLayout(mainLayout);

    topSplitterDefault = new QByteArray(topSplitter->saveState());

    readSettings();

    upgradeVersion();
  }


  CnetEditorWidget::~CnetEditorWidget()
  {
    writeSettings();

    if (topSplitterDefault)
    {
      delete topSplitterDefault;
      topSplitterDefault = NULL;
    }

    if (workingVersion)
    {
      delete workingVersion;
      workingVersion = NULL;
    }

    if (editPointDelegate)
    {
      delete editPointDelegate;
      editPointDelegate = NULL;
    }

    if (editMeasureDelegate)
    {
      delete editMeasureDelegate;
      editMeasureDelegate = NULL;
    }

    if (settingsPath)
    {
      delete settingsPath;
      settingsPath = NULL;
    }
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
    settingsPath = NULL;
    topSplitterDefault = NULL;
    workingVersion = NULL;
  }


  void CnetEditorWidget::setDriverView(int driverView)
  {
    View view = (View) driverView;
    pointModel->setDrivable(view == PointView);
    serialModel->setDrivable(view == SerialView);
    connectionModel->setDrivable(view == ConnectionView);

//     pointView->selectionModel()->clear();
//     pointView->collapseAll();

//     serialView->selectionModel()->clear();
//     serialView->collapseAll();
//
//     connectionView->selectionModel()->clear();
//     connectionView->collapseAll();

    syncFilterWidgets();
  }


  void CnetEditorWidget::activatePointView()
  {
    setDriverView(0);
  }


  void CnetEditorWidget::activateSerialView()
  {
    setDriverView(1);
  }


  void CnetEditorWidget::activateConnectionView()
  {
    setDriverView(2);
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

    connect(pointView, SIGNAL(activated()), serialView, SLOT(deactivate()));
    connect(pointView, SIGNAL(activated()), connectionView, SLOT(deactivate()));

    connect(serialView, SIGNAL(activated()), pointView, SLOT(deactivate()));
    connect(serialView, SIGNAL(activated()),
        connectionView, SLOT(deactivate()));

    connect(connectionView, SIGNAL(activated()), pointView, SLOT(deactivate()));
    connect(connectionView, SIGNAL(activated()),
        serialView, SLOT(deactivate()));

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
    pointView = new CnetView();
    pointView->setTitle("Point View");
    pointModel = new PointModel(controlNet, pointView, qApp);
    pointView->setModel(pointModel);
    connect(pointView, SIGNAL(activated()), this, SLOT(activatePointView()));
    connect(pointView, SIGNAL(selectionChanged()),
        this, SLOT(pointViewSelectionChanged()));
  }


  void CnetEditorWidget::createSerialView()
  {
    serialView = new CnetView();
    serialView->setTitle("Cube View");
    serialModel = new SerialModel(controlNet, serialView, qApp);
    serialView->setModel(serialModel);
    connect(serialView, SIGNAL(activated()), this, SLOT(activateSerialView()));
    connect(serialView, SIGNAL(selectionChanged()),
        this, SLOT(serialViewSelectionChanged()));
  }


  void CnetEditorWidget::createConnectionView()
  {
    connectionView = new CnetView();
    connectionView->setTitle("Cube Connection View");
    connectionModel = new ConnectionModel(controlNet, connectionView, qApp);
    connectionView->setModel(connectionModel);
    connect(connectionView, SIGNAL(activated()),
        this, SLOT(activateConnectionView()));
    connect(connectionView, SIGNAL(selectionChanged()),
        this, SLOT(connectionViewSelectionChanged()));
  }


  void CnetEditorWidget::createFilterArea()
  {
    ASSERT(pointModel);
    ASSERT(serialModel);
    ASSERT(connectionModel);

    pointFilterWidget = new FilterWidget("Points and Measures");
    if (pointModel)
    {
      pointModel->setFilter(pointFilterWidget);
    }

    serialFilterWidget = new FilterWidget("Images and Points");
    if (serialModel)
    {
      serialModel->setFilter(serialFilterWidget);
    }

    connectionFilterWidget = new FilterWidget("Connections");
    if (connectionModel)
    {
      connectionModel->setFilter(connectionFilterWidget);
    }

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

    for (int i = 0; i < PointTableModel::COLS; i++)
    {
      QAction * act = new QAction(
        PointTableModel::getColName((PointTableModel::Column) i), this);
      act->setCheckable(true);
      connect(act, SIGNAL(toggled(bool)), this, SLOT(pointColToggled()));
      editPointView->horizontalHeader()->addAction(act);
    }

    editPointView->horizontalHeader()->setContextMenuPolicy(
      Qt::ActionsContextMenu);

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

    for (int i = 0; i < MeasureTableModel::COLS; i++)
    {
      QAction * act = new QAction(
        MeasureTableModel::getColName((MeasureTableModel::Column) i), this);
      act->setCheckable(true);
      connect(act, SIGNAL(toggled(bool)), this, SLOT(measureColToggled()));
      editMeasureView->horizontalHeader()->addAction(act);
    }

    editMeasureView->horizontalHeader()->setContextMenuPolicy(
      Qt::ActionsContextMenu);

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


  void CnetEditorWidget::pointViewSelectionChanged()
  {
    QList< ControlPoint * > points;
    QList< ControlMeasure * > measures;
    QStringList cubeSerialNumbers;

    QList< AbstractTreeItem * > selectedItems = pointModel->getSelectedItems();

    for (int i = 0; i < selectedItems.size(); i++)
    {
      AbstractTreeItem * currentItem = selectedItems.at(i);
      AbstractTreeItem::InternalPointerType type =
        currentItem->getPointerType();

      if (type == AbstractTreeItem::Point)
      {
        ControlPoint * point =
          (*controlNet)[currentItem->getData()];
        points << point;

        // Grab all of the point's measures.
        for (int i = 0; i < currentItem->childCount(); i++)
          if (currentItem->childAt(i)->isVisible())
            measures <<
                (*point)[currentItem->childAt(i)->getData()];
      }
      else
        if (type == AbstractTreeItem::Measure)
        {
          QString pointId = currentItem->parent()->getData();
          QString serial = currentItem->getData();

          measures << controlNet->GetPoint(pointId)->GetMeasure(serial);
          cubeSerialNumbers << serial;
        }
        else
        {
          ASSERT(0);
        }
    }

    // populate editor tables
    editPointModel->setPoints(points);
    editMeasureModel->setMeasures(measures);

//    if (pointModel->isDrivable()) {
//      focusView(serialView, cubeSerialNumbers);
//      focusView(connectionView, cubeSerialNumbers);
//    }
//
//    updatingSelection = false;
  }


  void CnetEditorWidget::serialViewSelectionChanged()
  {
//     if (updatingSelection)
//       return;
//     updatingSelection = true;
//
//     updateTreeItemsWithNewSelection(newSelected, newDeselected);
//
//     QList< ControlPoint * > points;
//     QList< ControlMeasure * > measures;
//     QStringList pointIds;
//     QStringList cubeSerialNumbers;
//     QList< QModelIndex > indexes =
//         serialView->selectionModel()->selectedIndexes();
//     for (int i = 0; i < indexes.size(); i++)
//     {
//       AbstractTreeItem * item = static_cast< AbstractTreeItem * >(
//           indexes[i].internalPointer());
//       AbstractTreeItem::InternalPointerType type = item->pointerType();
//
//       if (type == AbstractTreeItem::CubeGraphNode)
//       {
//         cubeSerialNumbers << item->data().toString();
//       }
//       else
//       {
//         if (type == AbstractTreeItem::Point)
//         {
//           QString pointId = item->data().toString();
//           pointIds << pointId;
//
//           ControlPoint * point = controlNet->GetPoint(pointId);
//           points << point;
//           measures << point->GetMeasure(item->parent()->data().toString());
//         }
//         else
//         {
//           ASSERT(0);
//         }
//       }
//     }
//
//     editPointModel->setPoints(points);
//     editMeasureModel->setMeasures(measures);
//
//     if (serialModel->isDrivable())
//     {
// //       focusView(pointView, pointIds);
//       focusView(connectionView, cubeSerialNumbers);
//     }
//
//     updatingSelection = false;
  }



  void CnetEditorWidget::connectionViewSelectionChanged()
  {
//     if (updatingSelection)
//       return;
//     updatingSelection = true;
//
//     updateTreeItemsWithNewSelection(newSelected, newDeselected);
//
//     QList< ControlPoint * > points;
//     QList< ControlMeasure * > measures;
//     QStringList pointIds;
//     QStringList cubeSerialNumbers;
//     QList< QModelIndex > indexes =
//       connectionView->selectionModel()->selectedIndexes();
//     for (int i = 0; i < indexes.size(); i++)
//     {
//       AbstractTreeItem * item = static_cast< AbstractTreeItem * >(
//           indexes[i].internalPointer());
//       AbstractTreeItem::InternalPointerType type = item->pointerType();
//
//       if (type == AbstractTreeItem::CubeGraphNode)
//       {
//         cubeSerialNumbers << item->data().toString();
//       }
//       else
//       {
//         if (type == AbstractTreeItem::Point)
//         {
//           QString pointId = item->data().toString();
//           pointIds << pointId;
//
//           ControlPoint * point = controlNet->GetPoint(pointId);
//           points << point;
// //           measures << point->GetMeasure(item->parent()->data().toString()) <<
// //               point->GetMeasure(item->parent()->parent()->data().toString());
//         }
//         else
//         {
//           ASSERT(0);
//         }
//       }
//     }
//
//     editPointModel->setPoints(points);
//     editMeasureModel->setMeasures(measures);
//
//     if (connectionModel->isDrivable())
//     {
// //       focusView(pointView, pointIds);
//       focusView(serialView, cubeSerialNumbers);
//     }
//
//     updatingSelection = false;
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
    QTime timer;
    timer.start();
    cerr << "CnetEditorWidget::rebuildModels called\n";
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
    cerr << "CnetEditorWidget::rebuildModels done\n";
    cerr << "rebuildModels elapsed time: " << timer.elapsed() << "\n";
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


  void CnetEditorWidget::pointColToggled()
  {
    QList< QAction * > actions = editPointView->horizontalHeader()->actions();
    for (int i = 0; i < actions.size(); i++)
      editPointView->setColumnHidden(i, !actions[i]->isChecked());
  }


  void CnetEditorWidget::measureColToggled()
  {
    QList< QAction * > actions = editMeasureView->horizontalHeader()->actions();
    for (int i = 0; i < actions.size(); i++)
      editMeasureView->setColumnHidden(i, !actions[i]->isChecked());
  }


  void CnetEditorWidget::focusView(CnetView * view, QStringList labels)
  {
//     QAbstractItemModel * model = view->model();
//     for (int i = 0; i < model->rowCount(); i++)
//     {
//       QModelIndex index = model->index(i, 0, QModelIndex());
//       AbstractTreeItem * item = static_cast< AbstractTreeItem * >(
//           index.internalPointer());
//
//       bool hit = labels.contains(
//           model->data(index, Qt::DisplayRole).toString());
//       bool shouldExpand = hit && labels.size() == 1;
//
//       item->setSelected(hit);
//       view->selectionModel()->select(index, hit ? QItemSelectionModel::Select :
//           QItemSelectionModel::Deselect);
//
//       item->setExpanded(shouldExpand);
//       view->setExpanded(index, shouldExpand);
//
//       if (hit)
//         view->scrollTo(index, QAbstractItemView::PositionAtCenter);
//     }
  }


  void CnetEditorWidget::upgradeVersion()
  {
    if (*workingVersion == "")
    {
      topSplitter->restoreState(*topSplitterDefault);
      *workingVersion = "0.1";
    }

    if (*workingVersion != VERSION)
      upgradeVersion();
  }


  void CnetEditorWidget::readSettings()
  {
    ASSERT(topSplitter);
    ASSERT(workingVersion);
    ASSERT(settingsPath);
    ASSERT(editMeasureView);

    QSettings settings(*settingsPath, QSettings::NativeFormat);
    *workingVersion = settings.value("version", "").toString();
    setDriverView(settings.value("driverView", PointView).toInt());

    topSplitter->restoreState(settings.value("topSplitter").toByteArray());
    mainSplitter->restoreState(settings.value("mainSplitter").toByteArray());

    QList< QAction * > actions = editMeasureView->horizontalHeader()->actions();
    for (int i = 0; i < actions.size(); i++)
    {
      actions[i]->setChecked(settings.value("measure table column: " +
          MeasureTableModel::getColName((MeasureTableModel::Column) i),
          true).toBool());
    }

    actions = editPointView->horizontalHeader()->actions();
    for (int i = 0; i < actions.size(); i++)
    {
      actions[i]->setChecked(settings.value("point table column: " +
          PointTableModel::getColName((PointTableModel::Column) i),
          true).toBool());
    }
  }


  void CnetEditorWidget::writeSettings()
  {
    ASSERT(topSplitter);
    ASSERT(mainSplitter);
    ASSERT(settingsPath);
    ASSERT(editMeasureView);

    QSettings settings(*settingsPath, QSettings::NativeFormat);
    settings.setValue("version", VERSION);
    settings.setValue("topSplitter", topSplitter->saveState());
    settings.setValue("mainSplitter", mainSplitter->saveState());
    settings.setValue("driverView", getDriverView());

    QList< QAction * > actions = editMeasureView->horizontalHeader()->actions();
    for (int i = 0; i < actions.size(); i++)
    {
      settings.setValue("measure table column: " +
          MeasureTableModel::getColName((MeasureTableModel::Column) i),
          actions[i]->isChecked());
    }

    actions = editPointView->horizontalHeader()->actions();
    for (int i = 0; i < actions.size(); i++)
    {
      settings.setValue("point table column: " +
          PointTableModel::getColName((PointTableModel::Column) i),
          actions[i]->isChecked());
    }
  }
}
