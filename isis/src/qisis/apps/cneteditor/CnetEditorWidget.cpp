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
#include <QMessageBox>
#include <QModelIndex>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QSplitter>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>

#include "ControlNet.h"
#include "ControlPoint.h"
#include "iException.h"

#include "AbstractMeasureItem.h"
#include "AbstractPointItem.h"
#include "AbstractTreeItem.h"
#include "CnetMeasureTableModel.h"
#include "CnetPointTableModel.h"
#include "CnetTableView.h"
#include "CnetTableViewHeader.h"
#include "CnetTreeView.h"
#include "ConnectionModel.h"
#include "FilterWidget.h"
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


  CnetEditorWidget::CnetEditorWidget(ControlNet * cNet,
      QString pathForSettings)
  {
    nullify();

    workingVersion = new QString;
    menuActions = new QMap< QAction *, QList< QString > >;
    toolBarActions = new QMap< QString, QList< QAction * > >;

    updatingSelection = false;

    controlNet = cNet;
//     connect(controlNet, SIGNAL(networkStructureModified()),
//         this, SLOT(rebuildModels()));

    settingsPath = new QString(pathForSettings);

    QBoxLayout * mainLayout = createMainLayout();
    setLayout(mainLayout);

    createActions();
    
    topSplitterDefault = new QByteArray(topSplitter->saveState());

    readSettings();

    upgradeVersion();
  }


  CnetEditorWidget::~CnetEditorWidget()
  {
    writeSettings();

    delete topSplitterDefault;
    topSplitterDefault = NULL;

    delete workingVersion;
    workingVersion = NULL;

    delete settingsPath;
    settingsPath = NULL;

    delete pointTreeView;
    pointTreeView = NULL;

    delete serialTreeView;
    serialTreeView = NULL;

    delete connectionTreeView;
    connectionTreeView = NULL;
    
    delete pointTableView;
    pointTableView = NULL;
    
    delete measureTableView;
    measureTableView = NULL;

    delete pointFilterWidget;
    pointFilterWidget = NULL;

    delete serialFilterWidget;
    serialFilterWidget = NULL;

    delete connectionFilterWidget;
    connectionFilterWidget = NULL;
    
    delete menuActions;
    menuActions = NULL;

    delete toolBarActions;
    toolBarActions = NULL;
  }


  void CnetEditorWidget::nullify()
  {
    pointTreeView = NULL;
    serialTreeView = NULL;
    connectionTreeView = NULL;

    pointModel = NULL;
    serialModel = NULL;
    connectionModel = NULL;

    pointTableModel = NULL;
    measureTableModel = NULL;

    pointTableView = NULL;
    measureTableView = NULL;

    topSplitter = NULL;
    mainSplitter = NULL;

    menuActions = NULL;
    toolBarActions = NULL;
    
    filterArea = NULL;

    pointFilterWidget = NULL;
    serialFilterWidget = NULL;
    connectionFilterWidget = NULL;

    controlNet = NULL;
    settingsPath = NULL;
    topSplitterDefault = NULL;
    workingVersion = NULL;
  }


  void CnetEditorWidget::rebuildModels(QList<AbstractTreeItem *> itemsToDelete)
  {
    pointModel->stopWorking();
    serialModel->stopWorking();
    connectionModel->stopWorking();

    bool ignoreAll = false;
    foreach (AbstractTreeItem * item, itemsToDelete)
    {
//       cerr << "CnetEditorWidget::rebuildModels deleting "
//            << item->getPointer() << " (type is pt? "
//            << (item->getPointerType() == AbstractTreeItem::Point)
//            << ")\n";
      try
      {
        item->deleteSource();
      }
      catch (iException & e)
      {
        QString message = e.what();
        e.Clear();

        if (!ignoreAll)
        {
          if (item == itemsToDelete.last())
          {
            QMessageBox::warning(
                this, "Failed to delete row", message, QMessageBox::Ok);
          }
          else
          {
            message += "\n\nOkay to continue?";

            QMessageBox::StandardButton status = QMessageBox::warning(
                this, "Failed to delete row", message, QMessageBox::Yes |
                QMessageBox::YesToAll | QMessageBox::No);

            if (status == QMessageBox::YesToAll)
              ignoreAll = true;
            else if (status == QMessageBox::No)
              break;
          }
        }
      }
    }

    pointModel->rebuildItems();
    serialModel->rebuildItems();
    connectionModel->rebuildItems();
  }


  QBoxLayout * CnetEditorWidget::createMainLayout()
  {
    createPointTreeView();
    createSerialTreeView();
    createConnectionTreeView();

    connect(pointTreeView, SIGNAL(activated()),
        serialTreeView, SLOT(deactivate()));
    connect(pointTreeView, SIGNAL(activated()),
        connectionTreeView, SLOT(deactivate()));

    connect(serialTreeView, SIGNAL(activated()),
        pointTreeView, SLOT(deactivate()));
    connect(serialTreeView, SIGNAL(activated()),
        connectionTreeView, SLOT(deactivate()));

    connect(connectionTreeView, SIGNAL(activated()),
        pointTreeView, SLOT(deactivate()));
    connect(connectionTreeView, SIGNAL(activated()),
        serialTreeView, SLOT(deactivate()));

    createFilterArea();

    createPointTableView();
    QGroupBox * pointTableBox = new QGroupBox(tr("Control Point Table"));
    QHBoxLayout * pointTableLayout = new QHBoxLayout;
    pointTableLayout->addWidget(pointTableView);
    pointTableBox->setLayout(pointTableLayout);

    createMeasureTableView();
    QGroupBox * measureTableBox = new QGroupBox(tr("Control Measure Table"));
    QHBoxLayout * measureTableLayout = new QHBoxLayout;
    measureTableLayout->addWidget(measureTableView);
    measureTableBox->setLayout(measureTableLayout);

    topSplitter = new QSplitter(Qt::Horizontal);
//     topSplitter->setChildrenCollapsible(false);
//     topSplitter->addWidget(pointTreeView);
//     topSplitter->addWidget(serialTreeView);
//     topSplitter->addWidget(connectionTreeView);
//     topSplitter->addWidget(filterArea);

    mainSplitter = new QSplitter(Qt::Vertical);
    mainSplitter->addWidget(topSplitter);
    mainSplitter->addWidget(pointTableBox);
    mainSplitter->addWidget(measureTableBox);

    QBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->addWidget(mainSplitter);

    return mainLayout;
  }


  void CnetEditorWidget::createActions()
  {
    QAction * enableSortAct = new QAction(QIcon(":sort"),
                                          tr("&Enable Sorting"), this);
    enableSortAct->setCheckable(true);
    enableSortAct->setStatusTip(tr("Enable Sorting on Table Columns"));
    connect(enableSortAct, SIGNAL(toggled(bool)),
            this, SLOT(setSortingEnabled(bool)));
    
    ASSERT(menuActions);
    
    QList< QString > enableSortLocation;
    enableSortLocation.append(tr("Settings"));
    menuActions->insert(enableSortAct, enableSortLocation);
    
    QList< QAction * > actionList;
//     actionList.append(enableSortAct);
    
    QAction * freezeTablesAct = new QAction(tr("&Freeze Tables"), this);
    freezeTablesAct->setCheckable(true);
    freezeTablesAct->setToolTip(tr("Freeze tables (filters will not take "
                                   "effect until unfrozen)"));
    freezeTablesAct->setWhatsThis("When editing cells in the tables, it is ");
    connect(freezeTablesAct, SIGNAL(toggled(bool)),
            this, SLOT(setTablesFrozen(bool)));
    
    actionList.append(freezeTablesAct);
    toolBarActions->insert("settingsToolBar", actionList);
  }
  
  
  void CnetEditorWidget::createPointTreeView()
  {
    pointTreeView = new CnetTreeView();
    pointTreeView->setTitle("Point View");
    pointModel = new PointModel(controlNet, pointTreeView, qApp);
    pointTreeView->setModel(pointModel);
  }


  void CnetEditorWidget::createSerialTreeView()
  {
    serialTreeView = new CnetTreeView();
    serialTreeView->setTitle("Cube View");
    serialModel = new SerialModel(controlNet, serialTreeView, qApp);
    serialTreeView->setModel(serialModel);
  }


  void CnetEditorWidget::createConnectionTreeView()
  {
    connectionTreeView = new CnetTreeView();
    connectionTreeView->setTitle("Cube Connection View");
    connectionModel = new ConnectionModel(controlNet, connectionTreeView, qApp);
    connectionTreeView->setModel(connectionModel);
  }


  void CnetEditorWidget::createFilterArea()
  {
    ASSERT(pointModel);
    ASSERT(serialModel);
    ASSERT(connectionModel);

    FilterWidget * pointFilter = new FilterWidget("Points and Measures");
    if (pointModel)
    {
      pointModel->setFilter(pointFilter);
    }

    QHBoxLayout * pointFilterLayout = new QHBoxLayout;
    pointFilterLayout->addWidget(pointFilter);
    QWidget * pointArea = new QWidget;
    pointArea->setLayout(pointFilterLayout);
    QScrollArea * pointFilterScrollArea = new QScrollArea;
    pointFilterScrollArea->setWidget(pointArea);
    pointFilterScrollArea->setWidgetResizable(true);
    pointFilterWidget = pointFilterScrollArea;

    FilterWidget * serialFilter = new FilterWidget("Images and Points");
    if (serialModel)
    {
      serialModel->setFilter(serialFilter);
    }

    QHBoxLayout * serialFilterLayout = new QHBoxLayout;
    serialFilterLayout->addWidget(serialFilter);
    QWidget * serialArea = new QWidget;
    serialArea->setLayout(serialFilterLayout);
    QScrollArea * serialFilterScrollArea = new QScrollArea;
    serialFilterScrollArea->setWidget(serialArea);
    serialFilterScrollArea->setWidgetResizable(true);
    serialFilterWidget = serialFilterScrollArea;

    FilterWidget * connectionFilter = new FilterWidget("Connections");
    if (connectionModel)
    {
      connectionModel->setFilter(connectionFilter);
    }

    QHBoxLayout * connectionFilterLayout = new QHBoxLayout;
    connectionFilterLayout->addWidget(connectionFilter);
    QWidget * connectionArea = new QWidget;
    connectionArea->setLayout(connectionFilterLayout);
    QScrollArea * connectionFilterScrollArea = new QScrollArea;
    connectionFilterScrollArea->setWidget(connectionArea);
    connectionFilterScrollArea->setWidgetResizable(true);
    connectionFilterWidget = connectionFilterScrollArea;
  }


  void CnetEditorWidget::createPointTableView()
  {
    pointTableModel = new CnetPointTableModel(pointModel);
    pointTableView = new CnetTableView(pointTableModel, *settingsPath,
        "pointTableView");
    connect(pointTableView, SIGNAL(modelDataChanged()),
            this, SIGNAL(cnetModified()));
    
    connect(pointTreeView, SIGNAL(selectionChanged()),
            pointTableView, SLOT(handleModelSelectionChanged()));
    connect(pointTableView, SIGNAL(selectionChanged()),
            pointTreeView, SLOT(handleModelSelectionChanged()));
    
    connect(pointTableView, SIGNAL(rebuildModels(QList<AbstractTreeItem *>)),
            this, SLOT(rebuildModels(QList<AbstractTreeItem *>)));
    
    for (int i = 0; i < AbstractPointItem::COLS; i++)
    {
      QAction * act = new QAction(
          AbstractPointItem::getColumnName((AbstractPointItem::Column) i), this);
      act->setCheckable(true);
      connect(act, SIGNAL(toggled(bool)), this, SLOT(pointColToggled()));
      pointTableView->getHorizontalHeader()->addAction(act);
    }

    pointTableView->getHorizontalHeader()->setContextMenuPolicy(
        Qt::ActionsContextMenu);
  }


  void CnetEditorWidget::createMeasureTableView()
  {
    measureTableModel = new CnetMeasureTableModel(pointModel);
    measureTableView = new CnetTableView(measureTableModel, *settingsPath,
        "measureTableView");
    ASSERT(pointTableView);
    connect(pointTableView,
            SIGNAL(tableSelectionChanged(QList< AbstractTreeItem * >)),
            measureTableModel,
            SLOT(handleTreeSelectionChanged(QList<AbstractTreeItem*>)));

    connect(measureTableView,
            SIGNAL(tableSelectionChanged(QList< AbstractTreeItem * >)),
            pointTableModel,
            SLOT(handleTreeSelectionChanged(QList<AbstractTreeItem*>)));


    connect(measureTableView, SIGNAL(modelDataChanged()),
            this, SIGNAL(cnetModified()));
    connect(pointTreeView, SIGNAL(selectionChanged()),
            measureTableView, SLOT(handleModelSelectionChanged()));
    connect(measureTableView, SIGNAL(selectionChanged()),
            pointTreeView, SLOT(handleModelSelectionChanged()));
    connect(measureTableView, SIGNAL(rebuildModels(QList<AbstractTreeItem *>)),
            this, SLOT(rebuildModels(QList<AbstractTreeItem *>)));

    for (int i = 0; i < AbstractMeasureItem::COLS; i++)
    {
      QAction * act = new QAction(AbstractMeasureItem::getColumnName(
          (AbstractMeasureItem::Column) i), this);
      act->setCheckable(true);
      connect(act, SIGNAL(toggled(bool)), this, SLOT(measureColToggled()));
      measureTableView->getHorizontalHeader()->addAction(act);
    }

    measureTableView->getHorizontalHeader()->setContextMenuPolicy(
        Qt::ActionsContextMenu);
  }


  void CnetEditorWidget::rebuildModels()
  {
    rebuildModels(QList< AbstractTreeItem * >());
  }


  void CnetEditorWidget::pointColToggled()
  {
    QList< QAction * > actions =
        pointTableView->getHorizontalHeader()->actions();

    for (int i = 0; i < actions.size(); i++)
    {
      pointTableView->setColumnVisible(actions[i]->text(),
          actions[i]->isChecked());
    }
  }


  void CnetEditorWidget::measureColToggled()
  {
    QList< QAction * > actions =
        measureTableView->getHorizontalHeader()->actions();
    for (int i = 0; i < actions.size(); i++)
      measureTableView->setColumnVisible(actions[i]->text(),
          actions[i]->isChecked());
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
    ASSERT(measureTableView);

    QSettings settings(*settingsPath, QSettings::NativeFormat);
    *workingVersion = settings.value("version", "").toString();

    topSplitter->restoreState(settings.value("topSplitter").toByteArray());
    mainSplitter->restoreState(settings.value("mainSplitter").toByteArray());

    QString key;
    
    QList< QAction * > actions =
        measureTableView->getHorizontalHeader()->actions();
    for (int i = 0; i < actions.size(); i++)
    {
      key = measureTableView->objectName() + " " +
            AbstractMeasureItem::getColumnName((AbstractMeasureItem::Column) i);
      key.replace(" ", "_");
      actions[i]->setChecked(settings.value(key, true).toBool());
    }

    actions = pointTableView->getHorizontalHeader()->actions();
    for (int i = 0; i < actions.size(); i++)
    {
      key = pointTableView->objectName() + " " +
            AbstractPointItem::getColumnName((AbstractPointItem::Column) i);
      key.replace(" ", "_");
      actions[i]->setChecked(settings.value(key, true).toBool());
    }
  }


  void CnetEditorWidget::writeSettings()
  {
    ASSERT(topSplitter);
    ASSERT(mainSplitter);
    ASSERT(settingsPath);
    ASSERT(measureTableView);

    QSettings settings(*settingsPath, QSettings::NativeFormat);
    settings.setValue("version", VERSION);
    settings.setValue("topSplitter", topSplitter->saveState());
    settings.setValue("mainSplitter", mainSplitter->saveState());

    QString key;
    
    QList< QAction * > actions =
        measureTableView->getHorizontalHeader()->actions();
    for (int i = 0; i < actions.size(); i++)
    {
      key = measureTableView->objectName() + " " +
            AbstractMeasureItem::getColumnName((AbstractMeasureItem::Column) i);
      key.replace(" ", "_");
      settings.setValue(key, actions[i]->isChecked());
    }

    actions = pointTableView->getHorizontalHeader()->actions();
    for (int i = 0; i < actions.size(); i++)
    {
      key = pointTableView->objectName() + " " +
            AbstractPointItem::getColumnName((AbstractPointItem::Column) i);
      key.replace(" ", "_");
      settings.setValue(key, actions[i]->isChecked());
    }
  }

  QWidget * CnetEditorWidget::getPointTreeView()
  {
    return pointTreeView;
  }


  QWidget * CnetEditorWidget::getSerialTreeView()
  {
    return serialTreeView;
  }


  QWidget * CnetEditorWidget::getConnectionTreeView()
  {
    return connectionTreeView;
  }


  QWidget * CnetEditorWidget::getPointFilterWidget()
  {
    return pointFilterWidget;
  }


  QWidget * CnetEditorWidget::getSerialFilterWidget()
  {
    return serialFilterWidget;
  }


  QWidget * CnetEditorWidget::getConnectionFilterWidget()
  {
    return connectionFilterWidget;
  }


  QMap< QAction *, QList< QString > > CnetEditorWidget::getMenuActions()
  {
    ASSERT(menuActions);
    return *menuActions;
  }
  
  
  QMap< QString, QList< QAction * > > CnetEditorWidget::getToolBarActions()
  {
    ASSERT(toolBarActions);
    return *toolBarActions;
  }
  
  
  void CnetEditorWidget::setSortingEnabled(bool sortingIsEnabled)
  {
//     cerr << "CnetEditorWidget::setSortingEnabled called\n";
    ASSERT(pointTableModel);
    ASSERT(measureTableModel);
    
    if (pointTableModel)
      pointTableModel->setSortingEnabled(sortingIsEnabled);
    
    if (measureTableModel)
      measureTableModel->setSortingEnabled(sortingIsEnabled);
  }
  
  
  void CnetEditorWidget::setTablesFrozen(bool freezeTables)
  {
    if (freezeTables)
    {
      connectionModel->setFrozen(true);
      serialModel->setFrozen(true);
      pointModel->setFrozen(true);
    }
    else
    {
      pointModel->setFrozen(false);
      serialModel->setFrozen(false);
      connectionModel->setFrozen(false);
    }
  }
}

