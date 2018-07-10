/**
 * @file
 * $Date$
 * $Revision$
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
#include "IsisDebug.h"

#include "CnetEditorView.h"

#include <QAction>
#include <QGridLayout>
#include <QList>
#include <QMap>
#include <QMapIterator>
#include <QMenu>
#include <QMenuBar>
#include <QSize>
#include <QSizePolicy>
#include <QString>
#include <QTabWidget>
#include <QToolBar>
#include <QtXml>

#include "Control.h"
#include "ControlNet.h"
#include "CnetEditorWidget.h"
#include "Directory.h"
#include "FileName.h"
#include "Project.h"
#include "XmlStackedHandlerReader.h"
#include "ProjectItemViewMenu.h"

namespace Isis {
  /**
   * Constructor.
   */
  CnetEditorView::CnetEditorView(Directory *directory, Control *control, FileName configFile,
                                 QWidget *parent) : AbstractProjectItemView(parent) {

    // TODO: This layout should be inside of the cnet editor widget, but I put it here to not
    //     conflict with current work in the cnet editor widget code.
    //QWidget *result = new QWidget;

    QWidget *centralWidget = new QWidget;
    setCentralWidget(centralWidget);
    QGridLayout *resultLayout = new QGridLayout;
    centralWidget->setLayout(resultLayout);

    m_cnetEditorWidget = new CnetEditorWidget(control, configFile.expanded());
    m_control = control;

    resultLayout->addWidget(m_cnetEditorWidget, 0, 0, 1, 2);

    QTabWidget *treeViews = new QTabWidget;
    treeViews->addTab( m_cnetEditorWidget->pointTreeView(), tr("Point View") );
    treeViews->addTab( m_cnetEditorWidget->serialTreeView(), tr("Serial View") );
    treeViews->addTab( m_cnetEditorWidget->connectionTreeView(), tr("Connection View") );
    resultLayout->addWidget(treeViews, 1, 0, 1, 1);

    QTabWidget *filterViews = new QTabWidget;
    filterViews->addTab( m_cnetEditorWidget->pointFilterWidget(), tr("Filter Points and Measures") );
    filterViews->addTab( m_cnetEditorWidget->serialFilterWidget(), tr("Filter Images and Points") );
    filterViews->addTab( m_cnetEditorWidget->connectionFilterWidget(), tr("Filter Connections") );
    resultLayout->addWidget(filterViews, 1, 1, 1, 1);

    createMenus();
    createToolBars();

    // Store the actions for easy enable/disable.
    foreach (QAction *action, m_permToolBar->actions()) {
      addAction(action);
    }
    // On default, actions are disabled until the cursor enters the view.
    disableActions();
  }

  /**
   * Destructor
   */
  CnetEditorView::~CnetEditorView() {

    delete m_cnetEditorWidget;
    delete m_permToolBar;
    delete m_tablesMenu;

    m_tablesMenu = 0;
    m_permToolBar = 0;
  }

  /**
   * Uses the actions created by CnetEditorWidget, creates the tables menu,
   * and puts the actions into the tables menu.
   */
  void CnetEditorView::createMenus() {
    QMap< QAction *, QList< QString > > actionMap = m_cnetEditorWidget->menuActions();
    QMapIterator< QAction *, QList< QString > > actionMapIter(actionMap);

    m_tablesMenu = new ProjectItemViewMenu("&Tables");
    connect(m_tablesMenu, SIGNAL(menuClosed()), this, SLOT(disableActions()));
    menuBar()->addMenu(m_tablesMenu);

    while ( actionMapIter.hasNext() ) {
      actionMapIter.next();
      QAction *actionToAdd = actionMapIter.key();

      // Skip the "What's This?" action because it is in the main help menu of IPCE
      if (actionToAdd->text() == "What's This?") {
        continue;
      }
      m_tablesMenu->addAction(actionToAdd);
    }
  }


  /**
   * Uses and adds the actions created by CnetEditorWidget to the view's toolbars
   * Right now, all actions created in CnetEditorWidget are added to the toolpad.
   * This was copied from CnetEditorWindow
   */
  void CnetEditorView::createToolBars() {
    m_permToolBar = addToolBar("Standard Tools");
    m_permToolBar->setObjectName("permToolBar");
    m_permToolBar->setIconSize(QSize(22, 22));

    QMap< QString, QList< QAction * > > actionMap;
    actionMap = m_cnetEditorWidget->toolBarActions();
    QMapIterator< QString, QList< QAction * > > actionIter(actionMap);

    while (actionIter.hasNext()) {
      actionIter.next();
      QString objName = actionIter.key();
      QList< QAction * > actionList = actionIter.value();
      foreach (QAction *action, actionList) {
        m_permToolBar->addAction(action);
      }
    }
  }


  /**
   * Disables actions when cursor leaves the view. Overriden method
   * If a menu is visible, i.e. clicked on, this causes a leave event. We want the
   * actions to still be enabled when a menu is visible.
   *
   * @param event The leave event
   */
  void CnetEditorView::leaveEvent(QEvent *event) {
    if (!m_tablesMenu->isVisible()) {
      disableActions();
    }
  }


  /**
   * Returns the cnetEditorWidget.
   *
   * @return (cnetEditorWidget *) The cnetEditorWidget used to
   *         display the footprints.
   */
  CnetEditorWidget *CnetEditorView::cnetEditorWidget() {
    return m_cnetEditorWidget;
  }


  /**
   * @description Returns the Control displayed in the CnetEditorWidget
   *
   * @return (Control *) The Control displayed in the CnetEditorWidget
   */
  Control *CnetEditorView::control() {
    return m_control;
  }


  /**
   * This method pushes a new XmlHandler into the parser stack.
   *
   * @param xmlReader This is the parser stack.
   */
  void CnetEditorView::load(XmlStackedHandlerReader *xmlReader) {
    xmlReader->pushContentHandler(new XmlHandler(this));
  }


  /**
   * This method saves the Controls object ids to the stream.
   *
   * @param stream The stream that will output to directory.xml
   * @param project The project to save the users settings to
   * @param newProjectRoot New project's root directory
   */
  void CnetEditorView::save(QXmlStreamWriter &stream, Project *, FileName) const {

    stream.writeStartElement("cnetEditorView");
    stream.writeAttribute("objectName", objectName());
    stream.writeAttribute("id", m_control->id());
    stream.writeEndElement();
  }


  /**
   * Creates an XmlHandler for cnetEditor
   *
   * @param cnetEditor The widget to be serialized
   */
  CnetEditorView::XmlHandler::XmlHandler(CnetEditorView *cnetEditorView) {
    m_cnetEditorView = cnetEditorView;
  }


  /**
   * Destructor
   */
  CnetEditorView::XmlHandler::~XmlHandler() {
    delete m_cnetEditorView;
    m_cnetEditorView = NULL;
  }


  /**
   * Placeholder for later serialization of CnetEditorViews
   *
   * @param cnetEditor The CnetEditorView to be serialized
   * @param namespaceURI ???
   * @param localName Determines what attributes to retrieve from atts.
   * @param qName ???
   * @param atts Stores the attributes.
   *
   * @return @b bool The result of XmlStackedHandler's startElement() method.
   */
  bool CnetEditorView::XmlHandler::startElement(const QString &namespaceURI,
      const QString &localName, const QString &qName, const QXmlAttributes &atts) {

    bool result = XmlStackedHandler::startElement(namespaceURI, localName, qName, atts);
    return result;
  }


  /**
   * This method calls XmlStackedHandler's endElement() and dereferences pointers according to
   * the value of localName.
   *
   * @param namespaceURI ???
   * @param localName Determines which pointers to dereference.
   * @param qName ???
   *
   * @return @b bool The result of XmlStackedHandler's endElement() method.
   */
  bool CnetEditorView::XmlHandler::endElement(const QString &namespaceURI,
      const QString &localName, const QString &qName) {

    bool result = XmlStackedHandler::endElement(namespaceURI, localName, qName);
    return result;
  }
}
