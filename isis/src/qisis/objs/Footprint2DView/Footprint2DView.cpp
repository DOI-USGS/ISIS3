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

#include "Footprint2DView.h"

#include <QAction>
#include <QDockWidget>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QItemSelectionModel>
#include <QList>
#include <QSize>
#include <QSizePolicy>
#include <QStatusBar>
#include <QToolBar>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>
#include <QWidgetAction>
#include <QXmlStreamWriter>

#include "ControlPoint.h"
#include "Cube.h"
#include "Directory.h"
#include "Image.h"
#include "ImageFileListWidget.h"
#include "MosaicGraphicsView.h"
#include "MosaicSceneWidget.h"
#include "Project.h"
#include "ProjectItem.h"
#include "ProjectItemModel.h"
#include "Shape.h"
#include "ToolPad.h"
#include "XmlStackedHandlerReader.h"

namespace Isis {
  /**
   * Constructor.
   *
   * @param parent Pointer to parent QWidget
   */
  Footprint2DView::Footprint2DView(Directory *directory, QWidget *parent) :
                      AbstractProjectItemView(parent) {

    QStatusBar *statusBar = new QStatusBar(this);
    m_sceneWidget = new MosaicSceneWidget(statusBar, true, false, directory, this);
    m_sceneWidget->getScene()->installEventFilter(this);
    m_sceneWidget->setAcceptDrops(false);
    MosaicGraphicsView *graphicsView = m_sceneWidget->getView();
    graphicsView->installEventFilter(this);
    graphicsView->setAcceptDrops(false);

    connect( internalModel(), SIGNAL( itemAdded(ProjectItem *) ),
             this, SLOT( onItemAdded(ProjectItem *) ) );
    connect( internalModel(), SIGNAL( itemRemoved(ProjectItem *) ),
             this, SLOT( onItemRemoved(ProjectItem *) ) );

    connect(m_sceneWidget, SIGNAL(queueSelectionChanged() ),
            this, SLOT(onQueueSelectionChanged() ) );

    //  Pass on Signals emitted from ControlNetTool, through the MosaicSceneWidget
    //  TODO 2016-09-09 TLS Design:  Use a proxy model instead of signals?
    connect(m_sceneWidget, SIGNAL(modifyControlPoint(ControlPoint *)),
            this, SIGNAL(modifyControlPoint(ControlPoint *)));

    connect(m_sceneWidget, SIGNAL(deleteControlPoint(ControlPoint *)),
            this, SIGNAL(deleteControlPoint(ControlPoint *)));

    connect(m_sceneWidget, SIGNAL(createControlPoint(double, double)),
            this, SIGNAL(createControlPoint(double, double)));
    
    connect(m_sceneWidget, SIGNAL(mosCubeClosed(Image *)), 
            this, SLOT(onMosItemRemoved(Image *)));

    //  Pass on redrawMeasure signal from Directory, so the control measures are redrawn on all
    //  the footprints.
    connect(this, SIGNAL(redrawMeasures()), m_sceneWidget->getScene(), SLOT(update()));

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);
    QHBoxLayout *viewlayout = new QHBoxLayout();

    layout->addWidget(statusBar);

    m_fileListWidget = new ImageFileListWidget(directory);

    m_fileListWidget->setWindowTitle( tr("File List")  );
    m_fileListWidget->setObjectName( m_fileListWidget->windowTitle() );

    QDockWidget *imageFileListdock = new QDockWidget( m_fileListWidget->windowTitle() );
    imageFileListdock->setObjectName(imageFileListdock->windowTitle());
    imageFileListdock->setFeatures( QDockWidget::DockWidgetFloatable |
                                    QDockWidget::DockWidgetMovable);

    imageFileListdock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    imageFileListdock->setWidget(m_fileListWidget);

    m_window = new QMainWindow();
    m_window->addDockWidget(Qt::LeftDockWidgetArea, imageFileListdock, Qt::Vertical);
    m_window->setCentralWidget(m_sceneWidget);
    viewlayout->addWidget(m_window);
    layout->addLayout(viewlayout);

    setLayout(layout);

    m_permToolBar = new QToolBar("Standard Tools", 0);
    m_permToolBar->setObjectName("permToolBar");
    m_permToolBar->setIconSize(QSize(22, 22));
    //toolBarLayout->addWidget(m_permToolBar);

    m_activeToolBar = new QToolBar("Active Tool", 0);
    m_activeToolBar->setObjectName("activeToolBar");
    m_activeToolBar->setIconSize(QSize(22, 22));
    //toolBarLayout->addWidget(m_activeToolBar);

    m_toolPad = new ToolPad("Tool Pad", 0);
    m_toolPad->setObjectName("toolPad");
    //toolBarLayout->addWidget(m_toolPad);


    m_sceneWidget->addToPermanent(m_permToolBar);
    m_sceneWidget->addTo(m_activeToolBar);
    m_sceneWidget->addTo(m_toolPad);

    m_activeToolBarAction = new QWidgetAction(this);
    m_activeToolBarAction->setDefaultWidget(m_activeToolBar);

    setAcceptDrops(true);

    QSizePolicy policy = sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Expanding);
    policy.setVerticalPolicy(QSizePolicy::Expanding);
    setSizePolicy(policy);
  }


  /**
   * Destructor
   */
  Footprint2DView::~Footprint2DView() {
    delete m_fileListWidget;
    delete m_window;
    delete m_permToolBar;
    delete m_activeToolBar;
    delete m_toolPad;

    m_permToolBar = 0;
    m_activeToolBar = 0;
    m_toolPad = 0;
  }


  /**
   * Returns the suggested size for the widget.
   *
   * @return @b QSize The size
   */
  QSize Footprint2DView::sizeHint() const {
    return QSize(800, 600);
  }

  /**
   * Accessor for the MosaicSceneWidget
   */
  MosaicSceneWidget *Footprint2DView::mosaicSceneWidget() {
    return m_sceneWidget;
  }


  /**
   * Event filter to filter out drag and drop events.
   *
   * @param[in] watched (QObject *) The object being filtered
   * @param[in] event (QEvent *) The event
   *
   * @return @b bool True if the event was intercepted
   */
  bool Footprint2DView::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::DragEnter) {
      dragEnterEvent( static_cast<QDragEnterEvent *>(event) );
      return true;
    }
    else if (event->type() == QEvent::DragMove) {
      dragMoveEvent( static_cast<QDragMoveEvent *>(event) );
      return true;
    }
    else if (event->type() == QEvent::Drop) {
      dropEvent( static_cast<QDropEvent *>(event) );
      return true;
    }

    return AbstractProjectItemView::eventFilter(watched, event);
  }


  /**
   * Slot to connect to the itemAdded signal from the model. If the
   * item is an image it adds it to the scene.
   *
   * @param[in] item (ProjectItem *) The item
   */
  void Footprint2DView::onItemAdded(ProjectItem *item) {
    if (!item || (!item->isImage() && !item->isShape())) {
      return;
    }
    //TODO 2016-09-09 TLS  Handle Shapes-Create image from shape since qmos only handles images?
    //                   Still don't know if shape should inherit from image or contain an image?
    //
    Image *image;
    ImageList images;
    if (item->isShape()) {
      //TEMPORARY UNTIL SHAPE IS FIXED TO HOLD IMAGE, once Shape holds image go back to old code
      // previous to 10-21-16
      image = new Image(item->shape()->cube());
    }
    else if (item->isImage()) {
      image = item->image();
    }
    images.append(image);
    m_sceneWidget->addImages(images);
    m_fileListWidget->addImages(&images);

    if (!m_imageItemMap.value(image)) {
      m_imageItemMap.insert(image, item);
    }
  }

  
  /**
   * Slot at removes the mosaic item and corresponding image file list item when a cube is closed 
   * using the Close Cube context menu.
   * 
   * @param image The image that was closed and needs to be removed
   */
  void Footprint2DView::onMosItemRemoved(Image *image) {
    if (image) {
      ImageList images;
      images.append(image);

      m_sceneWidget->removeImages(images);
      m_fileListWidget->removeImages(&(images));

      if ( m_imageItemMap.value( image ) ) {
        m_imageItemMap.remove( image );
      }
    }
  }
  

  /**
   * Slot to connect to the itemRemoved signal from the model. If the item is an image it removes it
   * from the scene.
   *
   * @param[in] item (ProjectItem *) The item to be removed
   */
  void Footprint2DView::onItemRemoved(ProjectItem *item) {

    if (!item) {
      return;
    }

    if (item->isImage()) {

      ImageList images;
      images.append(item->image());

      m_sceneWidget->removeImages(images);
      m_fileListWidget->removeImages(&(images));

      if ( m_imageItemMap.value( item->image() ) ) {
        m_imageItemMap.remove( item->image());
      }
    }
  }


  /**
   * Slot to connect to the queueSelectionChanged signal from a
   * MosiacSceneWidget. Updates the selection in the model.
   */
  void Footprint2DView::onQueueSelectionChanged() {
    ImageList selectedImages = m_sceneWidget->selectedImages();

    if (selectedImages.isEmpty() ) {
      return;
    }

    Image *currentImage = selectedImages.first();

    internalModel()->selectionModel()->clear();

    if ( ProjectItem *item = m_imageItemMap.value(currentImage) ) {
      internalModel()->selectionModel()->setCurrentIndex(item->index(), QItemSelectionModel::Select);
    }

    foreach (Image *image, selectedImages) {
      if ( ProjectItem *item = m_imageItemMap.value(image) ) {
        internalModel()->selectionModel()->select(item->index(), QItemSelectionModel::Select);
      }
    }
  }


  /**
   * Returns a list of actions for the permanent tool bar.
   *
   * @return @b QList<QAction*> The actions
   */
  QList<QAction *> Footprint2DView::permToolBarActions() {
    return m_permToolBar->actions();
  }


  /**
   * Returns a list of actions for the active tool bar.
   *
   * @return @b QList<QAction*> The actions
   */
  QList<QAction *> Footprint2DView::activeToolBarActions() {
    QList<QAction *> actions;
    actions.append(m_activeToolBarAction);
    return actions;
  }


  /**
   * Returns a list of actions for the tool pad.
   *
   * @return @b QList<QAction*> The actions
   */
  QList<QAction *> Footprint2DView::toolPadActions() {
    return m_toolPad->actions();
  }


  /**
   * @brief Loads the Footprint2DView from an XML file.
   * @param xmlReader  The reader that takes in and parses the XML file.
   */
  void Footprint2DView::load(XmlStackedHandlerReader *xmlReader) {
    xmlReader->pushContentHandler( new XmlHandler(this) );
  }


  /**
   * @brief Save the footprint view widgets (ImageFileListWidget and MosaicSceneWidget to an XML 
   *        file.
   * @param stream  The XML stream writer
   * @param newProjectRoot The FileName of the project this Directory is attached to.
   *
   * @internal
   *   @history 2016-11-07 Ian Humphrey - Restored saving of footprints (footprint2view).
   *                           References #4486.
   */
  void Footprint2DView::save(QXmlStreamWriter &stream, Project *project,
                             FileName newProjectRoot) const {

    stream.writeStartElement("footprint2DView");

    m_fileListWidget->save(stream, project, newProjectRoot);
    m_sceneWidget->save(stream, project, newProjectRoot);

    stream.writeEndElement();
  }


  /**
   * @brief This function sets the Directory pointer for the Directory::XmlHandler class
   * @param directory The new directory we are setting XmlHandler's member variable to.
   */
  Footprint2DView::XmlHandler::XmlHandler(Footprint2DView *footprintView) {

    m_footprintView = footprintView;
  }


  /**
   * @brief The Destructor for Directory::XmlHandler
   */
  Footprint2DView::XmlHandler::~XmlHandler() {
  }


  /**
   * @brief The XML reader invokes this method at the start of every element in the
   * XML document.  This method expects <footprint2DView/> and <imageFileList/>
   * elements.
   * A quick example using this function:
   *     startElement("xsl","stylesheet","xsl:stylesheet",attributes)
   *
   * @param namespaceURI The Uniform Resource Identifier of the element's namespace
   * @param localName The local name string
   * @param qName The XML qualified string (or empty, if QNames are not available).
   * @param atts The XML attributes attached to each element
   * @return @b bool  Returns True signalling to the reader the start of a valid XML element.  If
   * False is returned, something bad happened.
   *
   */
  bool Footprint2DView::XmlHandler::startElement(const QString &namespaceURI, const QString &localName,
                                           const QString &qName, const QXmlAttributes &atts) {
    bool result = XmlStackedHandler::startElement(namespaceURI, localName, qName, atts);

    if (result) {
      if (localName == "mosaicScene") {
        m_footprintView->mosaicSceneWidget()->load(reader());
      }
      if (localName == "imageFileList") {
        m_footprintView->m_fileListWidget->load(reader());
      }
    }
    return result;
  }


  bool Footprint2DView::XmlHandler::endElement(const QString &namespaceURI,
      const QString &localName, const QString &qName) {
    bool result = XmlStackedHandler::endElement(namespaceURI, localName, qName);

    return result;
  }
}

