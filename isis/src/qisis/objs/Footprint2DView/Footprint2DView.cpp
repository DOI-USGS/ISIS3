/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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

#include "ControlNetTool.h"
#include "ControlPoint.h"
#include "Cube.h"
#include "Directory.h"
#include "Image.h"
#include "ImageFileListWidget.h"
#include "MosaicGraphicsView.h"
#include "MosaicSceneWidget.h"
#include "MosaicControlNetTool.h"
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
   * @param parent (QMainWindow *) Pointer to parent QMainWindow
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
    connect( internalModel(), SIGNAL( itemsAdded() ),
             this, SLOT( onItemsAdded() ) );
    connect( internalModel(), SIGNAL( itemRemoved(ProjectItem *) ),
             this, SLOT( onItemRemoved(ProjectItem *) ) );

    connect(m_sceneWidget, SIGNAL(queueSelectionChanged()),
            this, SLOT(onQueueSelectionChanged()), Qt::QueuedConnection);

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
    //  the footprints. Connection made in Directory from directory's signal to this signal since
    //  Directory doesn't have access to the scene within the sceneWidget.
    connect(this, SIGNAL(redrawMeasures()), m_sceneWidget->getScene(), SLOT(update()));

    setStatusBar(statusBar);

    m_fileListWidget = new ImageFileListWidget(directory);

    m_fileListWidget->setWindowTitle( tr("File List")  );
    m_fileListWidget->setObjectName( m_fileListWidget->windowTitle() );

    m_directory = directory;

    QDockWidget *imageFileListdock = new QDockWidget( m_fileListWidget->windowTitle() );
    imageFileListdock->setObjectName(imageFileListdock->windowTitle());
    imageFileListdock->setFeatures( QDockWidget::DockWidgetFloatable |
                                    QDockWidget::DockWidgetMovable);

    imageFileListdock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    imageFileListdock->setWidget(m_fileListWidget);

    addDockWidget(Qt::LeftDockWidgetArea, imageFileListdock, Qt::Vertical);
    setCentralWidget(m_sceneWidget);

    m_permToolBar = addToolBar("Standard Tools");
    m_permToolBar->setObjectName("permToolBar");
    m_permToolBar->setIconSize(QSize(22, 22));

    m_activeToolBar = addToolBar("Active Tool");
    m_activeToolBar->setObjectName("activeToolBar");
    m_activeToolBar->setIconSize(QSize(22, 22));

    m_toolPad = new ToolPad("Tool Pad", 0);
    m_toolPad->setObjectName("toolPad");
    addToolBar(Qt::RightToolBarArea, m_toolPad);

    m_sceneWidget->addToPermanent(m_permToolBar);
    m_sceneWidget->addTo(m_activeToolBar);
    m_sceneWidget->addTo(m_toolPad);

    // Store the actions for easy enable/disable.
    foreach (QAction *action, findChildren<QAction *>()) {
      addAction(action);
    }
    // On default, actions are disabled until the cursor enters the view.
    disableActions();
  }

  /**
   * Destructor
   */
  Footprint2DView::~Footprint2DView() {
    delete m_fileListWidget;
    delete m_permToolBar;
    delete m_activeToolBar;
    delete m_toolPad;

    m_permToolBar = 0;
    m_activeToolBar = 0;
    m_toolPad = 0;
  }


  /**
   * Accessor for the MosaicSceneWidget
   */
  MosaicSceneWidget *Footprint2DView::mosaicSceneWidget() {
    return m_sceneWidget;
  }


  /**
   * Accessor for the FileListWidget
   */
  ImageFileListWidget *Footprint2DView::fileListWidget() {
    return m_fileListWidget;
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
   * Slot to connect to the itemAdded signal from the model. If the item is an image or shape it is
   * added to a list. When everything has been added, then the list is added to the scene through
   * signal/slot connection from ProjectItemProxyModel signal, itemsAdded which is connected to
   * this objects onItemsAdded slot.
   *
   * @param[in] item (ProjectItem *) The item
   */
  void Footprint2DView::onItemAdded(ProjectItem *item) {
    if (!item || (!item->isImage() && !item->isShape())) {
      return;
    }

    Image *image;
    if (item->isShape()) {
      image = new Image(item->shape()->cube(), item->shape()->footprint(), item->shape()->id());
    }
    else if (item->isImage()) {
      image = item->image();
    }

    m_images.append(image);

    if (!m_imageItemMap.value(image)) {
      m_imageItemMap.insert(image, item);
    }
  }


  /**
   * Slot called once all selected images have been added to the proxy model.  This is much faster
   * than adding a single image at a time to the MosaicSceneWidget. This is connected from the
   * ProjectItemProxyModel::itemsAdded signal.
   *
   */
  void Footprint2DView::onItemsAdded() {
    //  This is called once all selected images have been added to proxy model (internalModel())
    //  This is much faster than adding a single image at a time to the scene widget
    m_sceneWidget->addImages(m_images);
    m_fileListWidget->addImages(&m_images);
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
   * A slot function that is called when directory emits a siganl that an active
   * control network is set. It enables the control network editor tool in the toolpad.
   *
   * @param value The boolean that holds if a control network has been set.
   */
  void Footprint2DView::enableControlNetTool(bool value) {
    foreach (QAction *action, m_toolPad->actions()) {
      if (action->toolTip() == "Control Net (c)") {
        action->setEnabled(value);
        if (value) {
          MosaicControlNetTool *cnetTool = static_cast<MosaicControlNetTool *>(action->parent());
          cnetTool->loadNetwork();
        }
      }
    }
  }


  /**
   * Enables toolbars and toolpad actions. Overriden method.
   * If an active control network has not been set, do not enable the cnet tool.
   */
  void Footprint2DView::enableActions() {
    foreach (QAction *action, actions()) {
      if (action->toolTip() == "Control Net (c)" && !m_directory->project()->activeControl()) {
        continue;
      }
      action->setEnabled(true);
    }
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
    stream.writeAttribute("objectName", objectName());

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
