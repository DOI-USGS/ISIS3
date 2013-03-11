/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
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
#include "Directory.h"

#include <QtDebug>

#include <QAction>
#include <QApplication>
#include <QDockWidget>
#include <QGridLayout>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QSplitter>
#include <QStringList>
#include <QXmlStreamWriter>

#include "CnetEditorViewWorkOrder.h"
#include "CnetEditorWidget.h"
#include "Control.h"
#include "ControlDisplayProperties.h"
#include "ControlList.h"
#include "CubeViewportViewWorkOrder.h"
#include "ExportControlNetWorkOrder.h"
#include "ExportImagesWorkOrder.h"
#include "FileName.h"
#include "Footprint2DViewWorkOrder.h"
#include "HistoryTreeWidget.h"
#include "IException.h"
#include "IString.h"
#include "ImageFileListViewWorkOrder.h"
#include "ImportControlNetWorkOrder.h"
#include "ImportImagesWorkOrder.h"
#include "ImageFileListWidget.h"
#include "MosaicSceneWidget.h"
#include "OpenProjectWorkOrder.h"
#include "Project.h"
#include "ProjectTreeWidget.h"
#include "RenameProjectWorkOrder.h"
#include "SaveProjectWorkOrder.h"
#include "SaveProjectAsWorkOrder.h"
#include "WarningTreeWidget.h"
#include "WorkOrder.h"
#include "Workspace.h"
#include "XmlStackedHandler.h"
#include "XmlStackedHandlerReader.h"

namespace Isis {

  Directory::Directory(QObject *parent) : QObject(parent) {

    try {
      m_project = new Project(*this);
    }
    catch (IException &e) {
      throw IException(e, IException::Programmer,
          "Could not create directory because Project could not be created.",
          _FILEINFO_);
    }

    connect(m_project, SIGNAL(imagesAdded(ImageList *)),
            this, SLOT(imagesAddedToProject(ImageList *)));

    m_projectTreeWidget = new ProjectTreeWidget(this);

    try {
      createWorkOrder<CnetEditorViewWorkOrder>();
      createWorkOrder<CubeViewportViewWorkOrder>();
      createWorkOrder<Footprint2DViewWorkOrder>();
      createWorkOrder<ImageFileListViewWorkOrder>();

      m_exportControlNetWorkOrder = createWorkOrder<ExportControlNetWorkOrder>();
      m_exportImagesWorkOrder = createWorkOrder<ExportImagesWorkOrder>();
      m_importControlNetWorkOrder = createWorkOrder<ImportControlNetWorkOrder>();
      m_importImagesWorkOrder = createWorkOrder<ImportImagesWorkOrder>();
      m_openProjectWorkOrder = createWorkOrder<OpenProjectWorkOrder>();
      m_saveProjectWorkOrder = createWorkOrder<SaveProjectWorkOrder>();
      m_saveProjectAsWorkOrder = createWorkOrder<SaveProjectAsWorkOrder>();

      m_renameProjectWorkOrder = createWorkOrder<RenameProjectWorkOrder>();
    }
    catch (IException &e) {
      throw IException(e, IException::Programmer,
          "Could not create directory because work orders are corrupt.",
           _FILEINFO_);
    }
  }


  Directory::~Directory() {
    delete m_projectTreeWidget;
    delete m_project;

    foreach (WorkOrder *workOrder, m_workOrders) {
      delete workOrder;
    }

    m_workOrders.clear();
  }


  void Directory::populateMainMenu(QMenuBar *menuBar) {
    QMenu *fileMenu = menuBar->findChild<QMenu *>("fileMenu");
    if (fileMenu) {
      fileMenu->addAction(m_importControlNetWorkOrder->clone());
      fileMenu->addAction(m_importImagesWorkOrder->clone());
      fileMenu->addSeparator();
      fileMenu->addAction(m_openProjectWorkOrder->clone());
      fileMenu->addSeparator();

      QAction *saveAction = m_saveProjectWorkOrder->clone();
      saveAction->setShortcut(Qt::Key_S | Qt::CTRL);

      connect(project()->undoStack(), SIGNAL(cleanChanged(bool)),
              saveAction, SLOT(setDisabled(bool)));

      fileMenu->addAction(saveAction);

      fileMenu->addAction(m_saveProjectAsWorkOrder->clone());

      fileMenu->addSeparator();
      fileMenu->addAction(m_exportControlNetWorkOrder->clone());
      fileMenu->addAction(m_exportImagesWorkOrder->clone());
      fileMenu->addSeparator();
    }

    QMenu *projectMenu = menuBar->findChild<QMenu *>("projectMenu");
    if (projectMenu) {
      projectMenu->addAction(m_renameProjectWorkOrder->clone());
    }
  }


  void Directory::setHistoryContainer(QDockWidget *historyContainer) {
    if (!m_historyTreeWidget) {
      m_historyTreeWidget = new HistoryTreeWidget(project());
    }
    historyContainer->setWidget(m_historyTreeWidget);
  }


  void Directory::setWarningContainer(QDockWidget *warningContainer) {
    if (!m_warningTreeWidget) {
      m_warningTreeWidget = new WarningTreeWidget;
    }
    warningContainer->setWidget(m_warningTreeWidget);
  }


  CnetEditorWidget *Directory::addCnetEditorView(Control *network) {
    QString title = tr("Cnet Editor View %1").arg(network->displayProperties()->displayName());

    FileName configFile("$HOME/.Isis/" + QApplication::applicationName() + "/" + title + ".config");

    // TODO: This layout should be inside of the cnet editor widget, but I put it here to not
    //     conflict with current work in the cnet editor widget code.
    QWidget *result = new QWidget;
    QGridLayout *resultLayout = new QGridLayout;
    result->setLayout(resultLayout);

    int row = 0;

    QMenuBar *menuBar = new QMenuBar;
    resultLayout->addWidget(menuBar, row, 0, 1, 2);
    row++;

    CnetEditorWidget *mainWidget =
        new CnetEditorWidget(network->controlNet(), configFile.expanded());
    resultLayout->addWidget(mainWidget, row, 0, 1, 2);
    row++;

    // Populate the menu...
    QMap< QAction *, QList< QString > > actionMap = mainWidget->menuActions();
    QMapIterator< QAction *, QList< QString > > actionMapIterator(actionMap);

    QMap<QString, QMenu *> topLevelMenus;

    while (actionMapIterator.hasNext()) {
      actionMapIterator.next();
      QAction *actionToAdd = actionMapIterator.key();
      QList< QString > location = actionMapIterator.value();

      QMenu *menuToPutActionInto = NULL;

      if (location.count()) {
        QString topLevelMenuTitle = location.takeFirst();
        if (!topLevelMenus[topLevelMenuTitle]) {
          topLevelMenus[topLevelMenuTitle] = menuBar->addMenu(topLevelMenuTitle);
        }

        menuToPutActionInto = topLevelMenus[topLevelMenuTitle];
      }

      foreach (QString menuName, location) {
        bool foundSubMenu = false;
        foreach (QAction *possibleSubMenu, menuToPutActionInto->actions()) {
          if (!foundSubMenu &&
              possibleSubMenu->menu() && possibleSubMenu->menu()->title() == menuName) {
            foundSubMenu = true;
            menuToPutActionInto = possibleSubMenu->menu();
          }
        }

        if (!foundSubMenu) {
          menuToPutActionInto = menuToPutActionInto->addMenu(menuName);
        }
      }

      menuToPutActionInto->addAction(actionToAdd);
    }

    QTabWidget *treeViews = new QTabWidget;
    treeViews->addTab(mainWidget->pointTreeView(), tr("Point View"));
    treeViews->addTab(mainWidget->serialTreeView(), tr("Serial View"));
    treeViews->addTab(mainWidget->connectionTreeView(), tr("Connection View"));
    resultLayout->addWidget(treeViews, row, 0, 1, 1);

    QTabWidget *filterViews = new QTabWidget;
    filterViews->addTab(mainWidget->pointFilterWidget(), tr("Filter Points and Measures"));
    filterViews->addTab(mainWidget->serialFilterWidget(), tr("Filter Images and Points"));
    filterViews->addTab(mainWidget->connectionFilterWidget(), tr("Filter Connections"));
    resultLayout->addWidget(filterViews, row, 1, 1, 1);
    row++;

    connect(result, SIGNAL(destroyed(QObject *)),
            this, SLOT(cleanupCnetEditorViewWidgets()));

    m_cnetEditorViewWidgets.append(mainWidget);

    result->setWindowTitle(title);
    result->setObjectName(title);

    emit newWidgetAvailable(result, Qt::RightDockWidgetArea, Qt::Horizontal);

    return mainWidget;
  }


  Workspace *Directory::addCubeDnView() {
    Workspace *result = new Workspace(true);

    connect(result, SIGNAL(destroyed(QObject *)),
            this, SLOT(cleanupCubeDnViewWidgets()));

    m_cubeDnViewWidgets.append(result);

    result->setWindowTitle(tr("Cube DN View %1").arg(m_cubeDnViewWidgets.count()));
    result->setObjectName(result->windowTitle());

    emit newWidgetAvailable(result, Qt::RightDockWidgetArea, Qt::Horizontal);

    return result;
  }


  MosaicSceneWidget *Directory::addFootprint2DView() {
    MosaicSceneWidget *result = new MosaicSceneWidget(NULL, true, true, this);

    connect(result, SIGNAL(destroyed(QObject *)),
            this, SLOT(cleanupFootprint2DViewWidgets()));

    m_footprint2DViewWidgets.append(result);

    result->setWindowTitle(tr("Footprint View %1").arg(m_footprint2DViewWidgets.count()));
    result->setObjectName(result->windowTitle());

    emit newWidgetAvailable(result, Qt::RightDockWidgetArea, Qt::Horizontal);

    return result;
  }


  ImageFileListWidget *Directory::addImageFileListView() {
    ImageFileListWidget *result = new ImageFileListWidget(this);

    connect(result, SIGNAL(destroyed(QObject *)),
            this, SLOT(cleanupFileListWidgets()));

    m_fileListWidgets.append(result);

    result->setWindowTitle(tr("File List %1").arg(m_fileListWidgets.count()));
    result->setObjectName(result->windowTitle());

    emit newWidgetAvailable(result, Qt::LeftDockWidgetArea, Qt::Horizontal);

    return result;
  }


  QWidget *Directory::warningWidget() {
    return m_warningTreeWidget;
  }


  QWidget *Directory::projectTreeWidget() {
    return m_projectTreeWidget;
  }


  void Directory::cleanupCnetEditorViewWidgets() {
    m_cnetEditorViewWidgets.removeAll(NULL);
  }


  void Directory::cleanupCubeDnViewWidgets() {
    m_cubeDnViewWidgets.removeAll(NULL);
  }


  void Directory::cleanupFileListWidgets() {
    m_fileListWidgets.removeAll(NULL);
  }


  void Directory::cleanupFootprint2DViewWidgets() {
    m_footprint2DViewWidgets.removeAll(NULL);
  }


  void Directory::imagesAddedToProject(ImageList *imageList) {
    m_projectTreeWidget->addImageGroup(imageList);
  }


  Project *Directory::project() const {
    return m_project;
  }


  QList<CnetEditorWidget *> Directory::cnetEditorViews() {
    QList<CnetEditorWidget *> results;

    foreach (CnetEditorWidget *widget, m_cnetEditorViewWidgets) {
      results.append(widget);
    }

    return results;
  }


  QList<Workspace *> Directory::cubeDnViews() {
    QList<Workspace *> results;

    foreach (Workspace *widget, m_cubeDnViewWidgets) {
      results.append(widget);
    }

    return results;
  }


  QList<MosaicSceneWidget *> Directory::footprint2DViews() {
    QList<MosaicSceneWidget *> results;

    foreach (MosaicSceneWidget *widget, m_footprint2DViewWidgets) {
      results.append(widget);
    }

    return results;
  }


  QList<ImageFileListWidget *> Directory::imageFileListViews() {
    QList<ImageFileListWidget *> results;

    foreach (ImageFileListWidget *widget, m_fileListWidgets) {
      results.append(widget);
    }

    return results;
  }


  QList<QProgressBar *> Directory::progressBars() {
    QList<QProgressBar *> result;
    return result;
  }


  void Directory::showWarning(QString text) {
    m_warningTreeWidget->showWarning(text);
  }


  QAction *Directory::redoAction() {
    return project()->undoStack()->createRedoAction(this);
  }


  QAction *Directory::undoAction() {
    return project()->undoStack()->createUndoAction(this);
  }


  void Directory::load(XmlStackedHandlerReader *xmlReader) {
    xmlReader->pushContentHandler(new XmlHandler(this));
  }


  void Directory::save(QXmlStreamWriter &stream, FileName newProjectRoot) const {
    stream.writeStartElement("directory");

    if (!m_fileListWidgets.isEmpty()) {
      stream.writeStartElement("fileListWidgets");

      foreach (ImageFileListWidget *fileListWidget, m_fileListWidgets) {
        fileListWidget->save(stream, project(), newProjectRoot);
      }

      stream.writeEndElement();
    }


    if (!m_footprint2DViewWidgets.isEmpty()) {
      stream.writeStartElement("footprintViews");

      foreach (MosaicSceneWidget *footprint2DViewWidget, m_footprint2DViewWidgets) {
        footprint2DViewWidget->save(stream, project(), newProjectRoot);
      }

      stream.writeEndElement();
    }

    stream.writeEndElement();
  }


  Directory::XmlHandler::XmlHandler(Directory *directory) {
    m_directory = directory;
  }


  Directory::XmlHandler::~XmlHandler() {
  }


  bool Directory::XmlHandler::startElement(const QString &namespaceURI, const QString &localName,
                                           const QString &qName, const QXmlAttributes &atts) {
    bool result = XmlStackedHandler::startElement(namespaceURI, localName, qName, atts);

    if (result) {
      if (localName == "footprint2DView") {
        m_directory->addFootprint2DView()->load(reader());
      }
      else if (localName == "imageFileList") {
        m_directory->addImageFileListView()->load(reader());
      }
    }

    return result;
  }


  /**
   * Reformat actionPairings to be user friendly for use in menus.
   *
   * actionPairings is:
   *    Widget A ->
   *      Action 1
   *      Action 2
   *      Action 3
   *    Widget B ->
   *      Action 1
   *      Action 3
   *      NULL
   *      Action 4
   *    ...
   *
   * We convert this into a list of actions, that when added to a menu, looks like:
   *   Action 1 -> Widget A
   *               Widget B
   *   Action 2 on Widget A
   *   Action 3 -> Widget A
   *               Widget B
   *   ----------------------
   *   Action 4 on Widget B
   *
   * The NULL separators aren't 100% yet, but work a good part of the time.
   *
   * This works by doing a data transformation and then using the resulting data structures
   *   to populate the menu.
   *
   * actionPairings is transformed into:
   *   restructuredData:
   *     Action 1 -> (Widget A, QAction *)
   *                 (Widget B, QAction *)
   *     Action 2 -> (Widget A, QAction *)
   *     Action 3 -> (Widget A, QAction *)
   *                 (Widget B, QAction *)
   *     Action 4 -> (Widget B, QAction *)
   *
   *   and
   *
   *   sortedActionTexts - A list of unique (if not empty) strings:
   *     "Action 1"
   *     "Action 2"
   *     "Action 3"
   *     ""
   *     "Action 4"
   *
   * This transformation is done by looping linearly through actionPairings and for each action in
   *   the pairings we add to the restructured data and append the action's text to
   *   sortedActionTexts.
   *
   * We loop through sortedActionTexts and populate the menu based based on the current (sorted)
   *   action text. If the action text is NULL (we saw a separator in the input), we add a NULL
   *   (separator) to the resulting list of actions. If it isn't NULL, we create a menu or utilize
   *   the action directly depending on if there are multiple actions with the same text.
   *
   */
  QList<QAction *> Directory::restructureActions(
      QList< QPair< QString, QList<QAction *> > > actionPairings) {
    QList<QAction *> results;

    QStringList sortedActionTexts;

    // This is a map from the Action Text to the actions and their widget titles
    QMap< QString, QList< QPair<QString, QAction *> > > restructuredData;

    QPair< QString, QList<QAction *> > singleWidgetPairing;
    foreach (singleWidgetPairing, actionPairings) {
      QString widgetTitle = singleWidgetPairing.first;
      QList<QAction *> widgetActions = singleWidgetPairing.second;

      foreach (QAction *widgetAction, widgetActions) {
        if (widgetAction) {
          QString actionText = widgetAction->text();

          restructuredData[actionText].append(qMakePair(widgetTitle, widgetAction));

          if (!sortedActionTexts.contains(actionText)) {
            sortedActionTexts.append(actionText);
          }
        }
        else {
          // Add separator
          if (!sortedActionTexts.isEmpty() && !sortedActionTexts.last().isEmpty()) {
            sortedActionTexts.append("");
          }
        }
      }
    }

    if (sortedActionTexts.count() && sortedActionTexts.last().isEmpty()) {
      sortedActionTexts.removeLast();
    }

    foreach (QString actionText, sortedActionTexts) {
      if (actionText.isEmpty()) {
        results.append(NULL);
      }
      else {
        // We know this list isn't empty because we always appended to the value when we
        //   accessed a particular key.
        QList< QPair<QString, QAction *> > actions = restructuredData[actionText];

        if (actions.count() == 1) {
          QAction *finalAct = actions.first().second;
          QString widgetTitle = actions.first().first;

          finalAct->setText(tr("%1 on %2").arg(actionText).arg(widgetTitle));
          results.append(finalAct);
        }
        else {
          QAction *menuAct = new QAction(actionText, NULL);

          QMenu *menu = new QMenu;
          menuAct->setMenu(menu);

          QList<QAction *> actionsInsideMenu;

          QPair<QString, QAction *> widgetTitleAndAction;
          foreach (widgetTitleAndAction, actions) {
            QString widgetTitle = widgetTitleAndAction.first;
            QAction *action = widgetTitleAndAction.second;

            action->setText(widgetTitle);
            actionsInsideMenu.append(action);
          }

          qSort(actionsInsideMenu.begin(), actionsInsideMenu.end(), &actionTextLessThan);

          QAction *allAct = new QAction(tr("All"), NULL);

          foreach (QAction *actionInMenu, actionsInsideMenu) {
            connect(allAct, SIGNAL(triggered()), actionInMenu, SIGNAL(triggered()));
            menu->addAction(actionInMenu);
          }

          menu->addSeparator();
          menu->addAction(allAct);

          results.append(menuAct);
        }
      }
    }

    return results;
  }


  bool Directory::actionTextLessThan(QAction *lhs, QAction *rhs) {
    return lhs->text().localeAwareCompare(rhs->text()) < 0;
  }
}
