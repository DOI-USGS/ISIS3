#include "IsisDebug.h"

#include "CnetEditorWidget.h"

#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QByteArray>
#include <QCloseEvent>
#include <QDebug>
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
#include <QWhatsThis>

#include "ControlNet.h"
#include "ControlPoint.h"
#include "IException.h"

#include "AbstractMeasureItem.h"
#include "AbstractPointItem.h"
#include "AbstractTreeItem.h"
#include "CnetDisplayProperties.h"
#include "MeasureTableModel.h"
#include "PointTableModel.h"
#include "TableView.h"
#include "TableViewHeader.h"
#include "TreeView.h"
#include "ImageImageTreeModel.h"
#include "FilterWidget.h"
#include "PointMeasureTreeModel.h"
#include "ImagePointTreeModel.h"


using std::cerr;


namespace Isis
{
  using namespace CnetViz;

//**************************************************************
//**************************************************************
//**************************************************************

  const QString CnetEditorWidget::VERSION = "0.1";

//**************************************************************
//**************************************************************
//**************************************************************


  CnetEditorWidget::CnetEditorWidget(ControlNet * cNet,
      QString pathForSettings) {
    nullify();

    workingVersion = new QString;
    menuActions = new QMap< QAction *, QList< QString > >;
    toolBarActions = new QMap< QString, QList< QAction * > >;

    updatingSelection = false;

    controlNet = cNet;
    connect(CnetDisplayProperties::getInstance(), SIGNAL(compositionFinished()),
            this, SLOT(rebuildModels()));
//     connect(controlNet, SIGNAL(networkStructureModified()),
//         this, SLOT(rebuildModels()));

    settingsPath = new QString(pathForSettings);

    QBoxLayout * mainLayout = createMainLayout();
    setLayout(mainLayout);

    createActions();

    readSettings();

    upgradeVersion();

    installEventFilter(this);
  }


  CnetEditorWidget::~CnetEditorWidget() {
    writeSettings();

    delete workingVersion;
    workingVersion = NULL;

    delete settingsPath;
    settingsPath = NULL;

    delete pointTreeView;
    pointTreeView = NULL;

    delete imageTreeView;
    imageTreeView = NULL;

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

    pointTableBox = NULL;
    measureTableBox = NULL;
    mainSplitter = NULL;

    // TODO: null all member widgets!
  }


  void CnetEditorWidget::nullify() {
    pointTreeView = NULL;
    imageTreeView = NULL;
    connectionTreeView = NULL;

    pointModel = NULL;
    imageModel = NULL;
    connectionModel = NULL;

    pointTableModel = NULL;
    measureTableModel = NULL;

    pointTableBox = NULL;
    measureTableBox = NULL;

    pointTableView = NULL;
    measureTableView = NULL;

    mainSplitter = NULL;

    menuActions = NULL;
    toolBarActions = NULL;

    filterArea = NULL;

    pointFilterWidget = NULL;
    serialFilterWidget = NULL;
    connectionFilterWidget = NULL;

    controlNet = NULL;
    settingsPath = NULL;
    workingVersion = NULL;
  }


  void CnetEditorWidget::rebuildModels(QList<AbstractTreeItem *> itemsToDelete) {
    pointModel->stopWorking();
    imageModel->stopWorking();
    connectionModel->stopWorking();

    bool ignoreAll = false;
    foreach (AbstractTreeItem * item, itemsToDelete) {
//       cerr << "CnetEditorWidget::rebuildModels deleting "
//            << item->getPointer() << " (type is pt? "
//            << (item->getPointerType() == AbstractTreeItem::Point)
//            << ")\n";
      try {
        item->deleteSource();
      }
      catch (IException &e) {
        QString message = e.what();

        if (!ignoreAll) {
          if (item == itemsToDelete.last()) {
            QMessageBox::warning(
                this, "Failed to delete row", message, QMessageBox::Ok);
          }
          else {
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
    imageModel->rebuildItems();
    connectionModel->rebuildItems();
  }


  QBoxLayout * CnetEditorWidget::createMainLayout() {
    createPointTreeView();
    createSerialTreeView();
    createConnectionTreeView();

    connect(pointTreeView, SIGNAL(activated()),
        imageTreeView, SLOT(deactivate()));
    connect(pointTreeView, SIGNAL(activated()),
        connectionTreeView, SLOT(deactivate()));

    connect(imageTreeView, SIGNAL(activated()),
        pointTreeView, SLOT(deactivate()));
    connect(imageTreeView, SIGNAL(activated()),
        connectionTreeView, SLOT(deactivate()));

    connect(connectionTreeView, SIGNAL(activated()),
        pointTreeView, SLOT(deactivate()));
    connect(connectionTreeView, SIGNAL(activated()),
        imageTreeView, SLOT(deactivate()));

    createFilterArea();

    createPointTableView();
    pointTableBox = new QGroupBox(tr("Control Point Table"));
    QHBoxLayout * pointTableLayout = new QHBoxLayout;
    pointTableLayout->addWidget(pointTableView);
    pointTableBox->setLayout(pointTableLayout);

    createMeasureTableView();
    measureTableBox = new QGroupBox(tr("Control Measure Table"));
    QHBoxLayout * measureTableLayout = new QHBoxLayout;
    measureTableLayout->addWidget(measureTableView);
    measureTableBox->setLayout(measureTableLayout);

    mainSplitter = new QSplitter(Qt::Vertical);
    mainSplitter->addWidget(pointTableBox);
    mainSplitter->addWidget(measureTableBox);

    QBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->addWidget(mainSplitter);

    return mainLayout;
  }


  void CnetEditorWidget::createActions() {
    ASSERT(menuActions);

    QAction * freezeTablesAct = new QAction(QIcon(":ice"),
                                            tr("&Freeze Tables"), this);
    freezeTablesAct->setCheckable(true);
    freezeTablesAct->setToolTip(tr("Freeze tables (filters will not take "
                                   "effect until unfrozen)"));
    freezeTablesAct->setStatusTip(tr("Freeze tables (filters will not take "
                                   "effect until unfrozen)"));
    freezeTablesAct->setWhatsThis(tr("<html>When frozen, the contents of the "
        "tables will be locked.  Current filters will not be applied to the "
        "tables until they are unfrozen.</html>"));
    connect(freezeTablesAct, SIGNAL(toggled(bool)),
            this, SLOT(setTablesFrozen(bool)));
    QList< QString > freezeTablesLocation;
    freezeTablesLocation.append(tr("&Tables"));
    menuActions->insert(freezeTablesAct, freezeTablesLocation);

    QAction * enableSortAct = new QAction(QIcon(":sort"),
                                          tr("&Enable Sorting"), this);
    enableSortAct->setCheckable(true);
    QString enableSortToolTipText = tr("Enable sorting on table columns");
    enableSortAct->setToolTip(enableSortToolTipText);
    enableSortAct->setStatusTip(enableSortToolTipText);
    enableSortAct->setWhatsThis(tr("<html>When sorting is enabled, the data "
        "in the tables can be sorted by clicking on column headings.  Sorting "
        "is disabled by default because of how long it can take for very large "
        "control networks.</html>"));
    connect(enableSortAct, SIGNAL(toggled(bool)),
            this, SLOT(setSortingEnabled(bool)));
    QList< QString > enableSortLocation;
    enableSortLocation.append(tr("&Tables"));
    menuActions->insert(enableSortAct, enableSortLocation);

    QAction * configureSortAct = new QAction(tr("&Sorting Options..."), this);
    QString configureSortToolTipText = tr("Configure table sorting options");
    enableSortAct->setToolTip(configureSortToolTipText);
    enableSortAct->setStatusTip(configureSortToolTipText);
    enableSortAct->setWhatsThis(tr("<html>Click here to configure options "
        "related to the sorting of table columns.</html>"));
    connect(configureSortAct, SIGNAL(toggled(bool)),
            this, SLOT(setSortingEnabled(bool)));
    //QList< QString > enableSortLocation;
    enableSortLocation.append(tr("&Tables"));
    menuActions->insert(enableSortAct, enableSortLocation);





    QAction * whatsThisAct = QWhatsThis::createAction(this);
    QList< QString > whatsThisLocation;
    whatsThisLocation.append(tr("&Help"));
    menuActions->insert(whatsThisAct, whatsThisLocation);

    QList< QAction * > tbActionList;
    tbActionList.append(freezeTablesAct);
    tbActionList.append(enableSortAct);
//     actionList.append(whatsThisAct);
    toolBarActions->insert("settingsToolBar", tbActionList);
  }


  void CnetEditorWidget::createPointTreeView() {
    pointTreeView = new TreeView();
    pointTreeView->setTitle("Point View");
    pointModel = new PointMeasureTreeModel(controlNet, pointTreeView, qApp);
    pointTreeView->setModel(pointModel);
  }


  void CnetEditorWidget::createSerialTreeView() {
    imageTreeView = new TreeView();
    imageTreeView->setTitle("Cube View");
    imageModel = new ImagePointTreeModel(controlNet, imageTreeView, qApp);
    imageTreeView->setModel(imageModel);
  }


  void CnetEditorWidget::createConnectionTreeView() {
    connectionTreeView = new TreeView();
    connectionTreeView->setTitle("Cube Connection View");
    connectionModel = new ImageImageTreeModel(controlNet, connectionTreeView, qApp);
    connectionTreeView->setModel(connectionModel);
  }


  void CnetEditorWidget::createFilterArea() {
    ASSERT(pointModel);
    ASSERT(imageModel);
    ASSERT(connectionModel);

    FilterWidget * pointFilter = new FilterWidget("Points and Measures");
    if (pointModel) {
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
    if (imageModel) {
      imageModel->setFilter(serialFilter);
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
    if (connectionModel) {
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


  void CnetEditorWidget::createPointTableView() {
    pointTableModel = new PointTableModel(pointModel);
    pointTableView = new TableView(pointTableModel, *settingsPath,
        "pointTableView");
    pointTableView->setWhatsThis("<html>Each row in the table is a control "
        "point.  Each column in the table is an attribute of a control "
        "point.<br/><br/>Cells that are gray are not editable.</html>");
    connect(pointTableView, SIGNAL(modelDataChanged()),
            this, SIGNAL(cnetModified()));

    connect(pointTreeView, SIGNAL(selectionChanged()),
            pointTableView, SLOT(handleModelSelectionChanged()));
    connect(pointTableView, SIGNAL(selectionChanged()),
            pointTreeView, SLOT(handleModelSelectionChanged()));

    connect(pointTableView,
            SIGNAL(rebuildModels(QList< CnetViz::AbstractTreeItem * >)),
            this,
            SLOT(rebuildModels(QList< CnetViz::AbstractTreeItem * >)));

    connect(pointTableView, SIGNAL(filterCountsChanged(int,int)),
            this, SLOT(handlePointTableFilterCountsChanged(int,int)));

    for (int i = 0; i < AbstractPointItem::COLS; i++) {
      QAction * act = new QAction(
          AbstractPointItem::getColumnName((AbstractPointItem::Column) i), this);
      act->setCheckable(true);
      connect(act, SIGNAL(toggled(bool)), this, SLOT(pointColToggled()));
      pointTableView->getHorizontalHeader()->addAction(act);
    }

    pointTableView->getHorizontalHeader()->setContextMenuPolicy(
        Qt::ActionsContextMenu);
  }


  void CnetEditorWidget::createMeasureTableView() {
    measureTableModel = new MeasureTableModel(pointModel);
    measureTableView = new TableView(measureTableModel, *settingsPath,
        "measureTableView");
    measureTableView->setWhatsThis("<html>Each row in the table is a control "
        "measure.  Each column in the table is an attribute of a control "
        "measure.<br/><br/>Rows with bold text are reference measures.  "
        "Cells that are gray are not editable.</html>");
    ASSERT(pointTableView);
    connect(pointTableView,
            SIGNAL(tableSelectionChanged(QList< AbstractTreeItem * >)),
            measureTableModel,
            SLOT(handleTreeSelectionChanged(QList<AbstractTreeItem*>)));

    connect(measureTableView,
            SIGNAL(tableSelectionChanged(QList< AbstractTreeItem * >)),
            pointTableModel,
            SLOT(handleTreeSelectionChanged(QList< AbstractTreeItem * >)));


    connect(measureTableView, SIGNAL(modelDataChanged()),
            this, SIGNAL(cnetModified()));
    connect(pointTreeView, SIGNAL(selectionChanged()),
            measureTableView, SLOT(handleModelSelectionChanged()));
    connect(measureTableView, SIGNAL(selectionChanged()),
            pointTreeView, SLOT(handleModelSelectionChanged()));
    connect(measureTableView,
            SIGNAL(rebuildModels(QList< CnetViz::AbstractTreeItem * >)),
            this,
            SLOT(rebuildModels(QList< CnetViz::AbstractTreeItem * >)));

    connect(measureTableView, SIGNAL(filterCountsChanged(int,int)),
            this, SLOT(handleMeasureTableFilterCountsChanged(int,int)));

    for (int i = 0; i < AbstractMeasureItem::COLS; i++) {
      QAction * act = new QAction(AbstractMeasureItem::getColumnName(
          (AbstractMeasureItem::Column) i), this);
      act->setCheckable(true);
      connect(act, SIGNAL(toggled(bool)), this, SLOT(measureColToggled()));
      measureTableView->getHorizontalHeader()->addAction(act);
    }

    measureTableView->getHorizontalHeader()->setContextMenuPolicy(
        Qt::ActionsContextMenu);
  }


  void CnetEditorWidget::rebuildModels() {
    rebuildModels(QList< AbstractTreeItem * >());
  }


  void CnetEditorWidget::pointColToggled() {
    QList< QAction * > actions =
        pointTableView->getHorizontalHeader()->actions();

    for (int i = 0; i < actions.size(); i++) {
      pointTableView->setColumnVisible(actions[i]->text(),
          actions[i]->isChecked());
    }
  }


  void CnetEditorWidget::measureColToggled() {
    QList< QAction * > actions =
        measureTableView->getHorizontalHeader()->actions();
    for (int i = 0; i < actions.size(); i++)
      measureTableView->setColumnVisible(actions[i]->text(),
          actions[i]->isChecked());
  }

  void CnetEditorWidget::handlePointTableFilterCountsChanged(
      int visibleRows, int totalRows) {
    handleTableFilterCountsChanged(visibleRows, totalRows, pointTableBox,
                                   "Control Point Table");
  }


  void CnetEditorWidget::handleMeasureTableFilterCountsChanged(
      int visibleRows, int totalRows) {
    handleTableFilterCountsChanged(visibleRows, totalRows, measureTableBox,
                                   "Control Measure Table");
  }


  void CnetEditorWidget::handleTableFilterCountsChanged(
      int visibleRows, int totalRows, QGroupBox * box, QString initialText) {
    if (box) {
      QString newTitle = initialText + " (";
      if (visibleRows > -1)
        newTitle += QString::number(visibleRows);
      else
        newTitle += "???";

      newTitle += " / " + QString::number(totalRows) + ")";

      box->setTitle(newTitle);
    }
  }


  void CnetEditorWidget::upgradeVersion() {
    if (*workingVersion == "") {
      *workingVersion = "0.1";
    }

    if (*workingVersion != VERSION)
      upgradeVersion();
  }


  void CnetEditorWidget::readSettings() {
    ASSERT(workingVersion);
    ASSERT(settingsPath);
    ASSERT(measureTableView);

    QSettings settings(*settingsPath, QSettings::NativeFormat);
    *workingVersion = settings.value("version", "").toString();

    mainSplitter->restoreState(settings.value("mainSplitter").toByteArray());

    QString key;

    QList< QAction * > actions =
        measureTableView->getHorizontalHeader()->actions();
    for (int i = 0; i < actions.size(); i++) {
      key = measureTableView->objectName() + " " +
            AbstractMeasureItem::getColumnName((AbstractMeasureItem::Column) i);
      key.replace(" ", "_");
      actions[i]->setChecked(settings.value(key, true).toBool());
    }

    actions = pointTableView->getHorizontalHeader()->actions();
    for (int i = 0; i < actions.size(); i++) {
      key = pointTableView->objectName() + " " +
            AbstractPointItem::getColumnName((AbstractPointItem::Column) i);
      key.replace(" ", "_");
      actions[i]->setChecked(settings.value(key, true).toBool());
    }
  }


  void CnetEditorWidget::writeSettings() {
    ASSERT(mainSplitter);
    ASSERT(settingsPath);
    ASSERT(measureTableView);

    QSettings settings(*settingsPath, QSettings::NativeFormat);
    settings.setValue("version", VERSION);
    settings.setValue("mainSplitter", mainSplitter->saveState());

    QString key;

    QList< QAction * > actions =
        measureTableView->getHorizontalHeader()->actions();
    for (int i = 0; i < actions.size(); i++) {
      key = measureTableView->objectName() + " " +
            AbstractMeasureItem::getColumnName((AbstractMeasureItem::Column) i);
      key.replace(" ", "_");
      settings.setValue(key, actions[i]->isChecked());
    }

    actions = pointTableView->getHorizontalHeader()->actions();
    for (int i = 0; i < actions.size(); i++) {
      key = pointTableView->objectName() + " " +
            AbstractPointItem::getColumnName((AbstractPointItem::Column) i);
      key.replace(" ", "_");
      settings.setValue(key, actions[i]->isChecked());
    }
  }

  QWidget * CnetEditorWidget::getPointTreeView() {
    return pointTreeView;
  }


  QWidget * CnetEditorWidget::getSerialTreeView() {
    return imageTreeView;
  }


  QWidget * CnetEditorWidget::getConnectionTreeView() {
    return connectionTreeView;
  }


  QWidget * CnetEditorWidget::getPointFilterWidget() {
    return pointFilterWidget;
  }


  QWidget * CnetEditorWidget::getSerialFilterWidget() {
    return serialFilterWidget;
  }


  QWidget * CnetEditorWidget::getConnectionFilterWidget() {
    return connectionFilterWidget;
  }


  QMap< QAction *, QList< QString > > CnetEditorWidget::getMenuActions() {
    ASSERT(menuActions);
    return *menuActions;
  }


  QMap< QString, QList< QAction * > > CnetEditorWidget::getToolBarActions() {
    ASSERT(toolBarActions);
    return *toolBarActions;
  }


  void CnetEditorWidget::setSortingEnabled(bool sortingIsEnabled) {
//     cerr << "CnetEditorWidget::setSortingEnabled called\n";
    ASSERT(pointTableModel);
    ASSERT(measureTableModel);

    if (pointTableModel)
      pointTableModel->setSortingEnabled(sortingIsEnabled);

    if (measureTableModel)
      measureTableModel->setSortingEnabled(sortingIsEnabled);
  }


  void CnetEditorWidget::setTablesFrozen(bool freezeTables) {
    if (freezeTables) {
      connectionModel->setFrozen(true);
      imageModel->setFrozen(true);
      pointModel->setFrozen(true);
    }
    else {
      pointModel->setFrozen(false);
      imageModel->setFrozen(false);
      connectionModel->setFrozen(false);
    }
  }
}

