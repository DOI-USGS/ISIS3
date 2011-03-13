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
#include "PointChildItem.h"
#include "PointTableDelegate.h"
#include "PointTableModel.h"
#include "PointModel.h"
#include "SerialModel.h"
#include "TreeItem.h"


using std::cerr;


namespace Isis
{

  CnetEditorWidget::CnetEditorWidget(Isis::ControlNet * cNet)
  {
    nullify();
    updatingSelection = false;
    controlNet = cNet;

    QBoxLayout * mainLayout = createMainLayout();
    setSynchronizedViews(false);
    setLayout(mainLayout);
  }


  CnetEditorWidget::~CnetEditorWidget()
  {}


  void CnetEditorWidget::setSynchronizedViews(bool synchronized)
  {
    synchronizeViews = synchronized;

    if (synchronizeViews)
    {
      serialView->setSelectionMode(QAbstractItemView::MultiSelection);
      connectionView->setSelectionMode(QAbstractItemView::MultiSelection);
      pointViewSelectionChanged();
    }
    else
    {
      serialView->setSelectionMode(QAbstractItemView::NoSelection);
      connectionView->setSelectionMode(QAbstractItemView::NoSelection);
      serialView->selectionModel()->clear();
      connectionView->selectionModel()->clear();
    }
  }


  QBoxLayout * CnetEditorWidget::createMainLayout()
  {
    pointView = new QTreeView();
    pointModel = new PointModel(controlNet, qApp);
    pointView->setModel(pointModel);
    connect(pointView->selectionModel(), SIGNAL(selectionChanged(
        const QItemSelection &, const QItemSelection &)), this,
        SLOT(pointViewSelectionChanged()));
//     pointView->setExpandsOnDoubleClick(false);
    pointView->setAlternatingRowColors(true);
    pointView->setSelectionMode(QAbstractItemView::MultiSelection);

    serialView = new QTreeView();
    serialModel = new SerialModel(controlNet, qApp);
    serialView->setModel(serialModel);
    connect(serialView->selectionModel(), SIGNAL(selectionChanged(
        const QItemSelection &, const QItemSelection &)), this,
        SLOT(serialViewSelectionChanged()));
//     serialView->setExpandsOnDoubleClick(false);
    serialView->setAlternatingRowColors(true);

    connectionView = new QTreeView();
    connectionModel = new ConnectionModel(controlNet, qApp);
    connectionView->setModel(connectionModel);
    connect(connectionView->selectionModel(), SIGNAL(selectionChanged(
        const QItemSelection &, const QItemSelection &)), this,
        SLOT(connectionViewSelectionChanged()));
//     connectionView->setExpandsOnDoubleClick(false);
    connectionView->setAlternatingRowColors(true);

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

//     QHeaderView * vheader = editPointView->verticalHeader();
//     vheader->setContextMenuPolicy(Qt::ActionsContextMenu);
//     QAction * removeAction = new QAction("Remove", this);
//     vheader->addAction(removeAction);

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


    QSplitter * topSplitter = new QSplitter(Qt::Horizontal);
    topSplitter->addWidget(pointView);
    topSplitter->addWidget(serialView);
    topSplitter->addWidget(connectionView);

    QSplitter * mainSplitter = new QSplitter(Qt::Vertical);
    mainSplitter->addWidget(topSplitter);
    mainSplitter->addWidget(editPointBox);
    mainSplitter->addWidget(editMeasureBox);
//     mainSplitter->setStretchFactor(0, 2);
//     mainSplitter->setStretchFactor(1, 1);
//     mainSplitter->setStretchFactor(2, 1);

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

    editPointView = NULL;
    editMeasureView = NULL;

    controlNet = NULL;
  }


  void CnetEditorWidget::pointViewSelectionChanged()
  {
    // If this method was called because it is synchronized to another view
    // that got clicked then do nothing.  The *viewSelectionChanged for the
    // clicked view is already handling how this view should be updated.
    if (updatingSelection)
      return;
    updatingSelection = true;


    QList< ControlPoint * > points;
    QList< ControlMeasure * > measures;
    QStringList cubeSerialNumbers;
    QList< QModelIndex > indexes =
      pointView->selectionModel()->selectedIndexes();
    for (int i = 0; i < indexes.size(); i++)
    {
      TreeItem * item = static_cast< TreeItem * >(
          indexes[i].internalPointer());
      TreeItem::InternalPointerType type = item->pointerType();

      if (type == TreeItem::Point)
      {
        points << controlNet->GetPoint(item->data(0).toString());
      }
      else
      {
        if (type == TreeItem::Measure)
        {
          QString pointId = item->parent()->data(0).toString();
          QString serial = item->data(0).toString();

          measures << controlNet->GetPoint(pointId)->GetMeasure(serial);
          cubeSerialNumbers << serial;
        }
        else
        {
          // if this is reachable code then we have a serious problem!
          cerr << "error in CnetEditorWidget::pointViewSelectionChanged!\n";
        }
      }
    }

    //********************FIXME***************************************/
    // populate editor tables
    editPointModel->setPoints(points);
    QList< ControlMeasure * > allMeasuresForSelectedPoints;
    foreach(ControlPoint * point, points)
    {
      for (int i = 0; i < point->GetNumMeasures(); i++)
        allMeasuresForSelectedPoints.append((*point)[i]);
    }
    editMeasureModel->setMeasures(allMeasuresForSelectedPoints);
    //********************FIXME***************************************/


    if (synchronizeViews)
    {
      focusView(serialView, cubeSerialNumbers);
      focusView(connectionView, cubeSerialNumbers);
    }

    updatingSelection = false;
  }


  void CnetEditorWidget::serialViewSelectionChanged()
  {
    if (updatingSelection || !synchronizeViews)
      return;
    updatingSelection = true;


    QList< ControlPoint * > points;
    QList< ControlMeasure * > measures;
    QStringList pointIds;
    QStringList cubeSerialNumbers;
    QList< QModelIndex > indexes =
      serialView->selectionModel()->selectedIndexes();
    for (int i = 0; i < indexes.size(); i++)
    {
      TreeItem * item = static_cast< TreeItem * >(
          indexes[i].internalPointer());
      TreeItem::InternalPointerType type = item->pointerType();

      if (type == TreeItem::Serial)
      {
        cubeSerialNumbers << item->data(0).toString();
      }
      else
      {
        if (type == TreeItem::Point)
        {
          QString pointId = item->data(0).toString();
          pointIds << pointId;

          ControlPoint * point = controlNet->GetPoint(pointId);
          points << point;
          measures << point->GetMeasure(item->parent()->data(0).toString());
        }
        else
        {
          // This is not an error case!  This code will not be reachable
          // if things are working correctly.  This is only here to catch
          // the unlikely event that I made a very serious error.
          cerr << "error in CnetEditorWidget::pointViewSelectionChanged!\n";
        }
      }
    }

    editPointModel->setPoints(points);
    editMeasureModel->setMeasures(measures);

    focusView(pointView, pointIds);
    focusView(connectionView, cubeSerialNumbers);

    updatingSelection = false;
  }



  void CnetEditorWidget::connectionViewSelectionChanged()
  {
    // If this method was called because it is synchronized to another view
    // that got clicked then do nothing.  The *viewSelectionChanged for the
    // clicked view is already handling how this view should be updated.
    if (updatingSelection)
      return;
    updatingSelection = true;


    QList< ControlPoint * > points;
    QList< ControlMeasure * > measures;
    QStringList pointIds;
    QStringList cubeSerialNumbers;
    QList< QModelIndex > indexes =
      connectionView->selectionModel()->selectedIndexes();
    for (int i = 0; i < indexes.size(); i++)
    {
      TreeItem * item = static_cast< TreeItem * >(
          indexes[i].internalPointer());
      TreeItem::InternalPointerType type = item->pointerType();

      if (type == TreeItem::ConnectionParent || type == TreeItem::Serial)
      {
        cubeSerialNumbers << item->data(0).toString();
      }
      else
      {
        if (type == TreeItem::Point)
        {
          QString pointId = item->data(0).toString();
          pointIds << pointId;

          ControlPoint * point = controlNet->GetPoint(pointId);
          points << point;
//           measures << point->GetMeasure(item->parent()->data(0).toString()) <<
//               point->GetMeasure(item->parent()->parent()->data(0).toString());
        }
        else
        {
          // This is not an error case!  This code will not be reachable
          // if things are working correctly.  This is only here to catch
          // the unlikely event that I made a very serious error.
          cerr << "error in CnetEditorWidget::connectionViewSelectionChanged!"
              "\n";
        }
      }
    }

    if (synchronizeViews)
    {
      editPointModel->setPoints(points);
      editMeasureModel->setMeasures(measures);

      focusView(pointView, pointIds);
      focusView(serialView, cubeSerialNumbers);
    }

    updatingSelection = false;
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
        view->selectionModel()->select(index,
            QItemSelectionModel::Select);
        if (labels.size() == 1)
          view->setExpanded(index, true);
        view->scrollTo(index, QAbstractItemView::PositionAtCenter);
      }
    }
  }
}
