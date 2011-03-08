#include "IsisDebug.h"

#include "CnetEditorWidget.h"

#include <iostream>

#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelection>
#include <QModelIndex>
#include <QTableView>
#include <QTreeView>
#include <QSplitter>
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
    setLayout(mainLayout);
  }


  CnetEditorWidget::~CnetEditorWidget()
  {}


  QBoxLayout * CnetEditorWidget::createMainLayout()
  {
    pointView = new QTreeView();
    pointModel = new PointModel(controlNet, qApp);
    pointView->setModel(pointModel);
    connect(pointView->selectionModel(), SIGNAL(selectionChanged(
        const QItemSelection &, const QItemSelection &)), this,
        SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));
    pointView->setExpandsOnDoubleClick(false);
    pointView->setAlternatingRowColors(true);

    serialView = new QTreeView();
    serialModel = new SerialModel(controlNet, qApp);
    serialView->setModel(serialModel);
    connect(serialView->selectionModel(), SIGNAL(
        selectionChanged(const QItemSelection &, const QItemSelection &)),
        this,
        SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));
    serialView->setExpandsOnDoubleClick(false);
    serialView->setAlternatingRowColors(true);
    
    connectionView = new QTreeView();
    connectionModel = new ConnectionModel(controlNet, qApp);
    connectionView->setModel(connectionModel);
    connect(connectionView->selectionModel(), SIGNAL(
        selectionChanged(const QItemSelection &, const QItemSelection &)),
        this,
        SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));
    connectionView->setExpandsOnDoubleClick(false);
    connectionView->setAlternatingRowColors(true);
    
    editPointView = new QTableView();
    editPointModel = new PointTableModel(qApp);
    editPointView->setModel(editPointModel);
    connect(editPointModel, SIGNAL(dataChanged(const QModelIndex &,
        const QModelIndex &)), editPointView, SLOT(resizeColumnsToContents()));
    editPointDelegate = new PointTableDelegate(editPointModel);
    connect(editPointDelegate, SIGNAL(dataEdited()),
        this, SIGNAL(cnetModified()));
    editPointView->setItemDelegate(editPointDelegate);
    editPointView->resizeColumnsToContents();
    editPointView->setEditTriggers(QAbstractItemView::AllEditTriggers);
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
    editMeasureDelegate = new MeasureTableDelegate(editMeasureModel);
    connect(editMeasureDelegate, SIGNAL(dataEdited()), 
        this, SIGNAL(cnetModified()));
    editMeasureView->setItemDelegate(editMeasureDelegate);
    editMeasureView->resizeColumnsToContents();
    editMeasureView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    

    QSplitter * topSplitter = new QSplitter(Qt::Horizontal);
    topSplitter->addWidget(pointView);
    topSplitter->addWidget(serialView);
    topSplitter->addWidget(connectionView);

    QSplitter * mainSplitter = new QSplitter(Qt::Vertical);
    mainSplitter->addWidget(topSplitter);
    mainSplitter->addWidget(editPointView);
    mainSplitter->addWidget(editMeasureView);
    mainSplitter->setStretchFactor(0, 2);
    mainSplitter->setStretchFactor(1, 1);
    mainSplitter->setStretchFactor(2, 1);

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


  void CnetEditorWidget::selectionChanged(const QItemSelection & selected,
      const QItemSelection & deselected)
  {
    // DISABLE RECURSIVE CALLS!!!
    // this method can and will be called INDIRECTLY by this method.
    if (updatingSelection)
      return;
    
    updatingSelection = true;
    

    QList< QModelIndex > indices = selected.indexes();

    if (indices.size() == 1)
    {
      TreeItem * item = static_cast< TreeItem * >(
          indices[0].internalPointer());

      TreeItem::InternalPointerType type = item->pointerType();
      QString itemLabel = item->data(0).toString();
      switch (type)
      {
        case TreeItem::Point:
          editPointModel->setPoint(controlNet->GetPoint(itemLabel));
          if (dynamic_cast< PointChildItem * >(item))
          {
            pointView->selectionModel()->clear();
            focusView(pointView, itemLabel);
          }
          break;
          
        case TreeItem::Measure:
          {
            QString parentLabel = item->parent()->data(0).toString();
            editMeasureModel->setMeasure(controlNet->GetPoint(
                parentLabel)->GetMeasure(itemLabel));
            serialView->selectionModel()->clear();
            focusView(serialView, itemLabel);
            connectionView->selectionModel()->clear();
            focusView(connectionView, itemLabel);
          }
          break;
          
        case TreeItem::ConnectionParent:
          serialView->selectionModel()->clear();
          focusView(serialView, itemLabel);
          break;
          
        case TreeItem::Serial:
          if (item->parent())
          {
            serialView->selectionModel()->clear();
            focusView(serialView, itemLabel);
          }
          else
          {
            connectionView->selectionModel()->clear();
            focusView(connectionView, itemLabel);
          }
          break;
      }
    }
    updatingSelection = false;
  }
  
  
  void CnetEditorWidget::focusView(QTreeView * view, QString label)
  {
    QAbstractItemModel * model = view->model();
    view->selectionModel()->clear();
    view->collapseAll();
    for (int i = 0; i < model->rowCount(); i++)
    {
      QModelIndex index = model->index(i, 0, QModelIndex());
      if (model->data(index, Qt::DisplayRole).toString() == label)
      {
        view->selectionModel()->select(index,
            QItemSelectionModel::SelectCurrent);
        view->setExpanded(index, true);
        view->scrollTo(index, QAbstractItemView::PositionAtCenter);
      }
    }    
  }
}
