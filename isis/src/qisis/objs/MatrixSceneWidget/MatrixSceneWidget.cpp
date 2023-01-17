#include "MatrixSceneWidget.h"

#include <float.h>
#include <sstream>

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QList>
#include <QMap>
#include <QPen>
#include <QtCore>
#include <QtWidgets>
#include <QtXml>
#include <QTransform>

#include "CorrelationMatrix.h"
#include "Directory.h"
#include "Distance.h"
#include "FileName.h"
#include "GraphicsView.h"
#include "IException.h"
#include "IString.h"
#include "MatrixGraphicsScene.h"
#include "MatrixGraphicsView.h"
#include "MatrixOptions.h"
#include "ProgressBar.h"
#include "Project.h"
#include "PvlObject.h"
#include "Pvl.h"
#include "SparseBlockMatrix.h"
#include "TextFile.h"
#include "Target.h"
#include "ToolPad.h"
#include "XmlStackedHandlerReader.h"

#include <boost/numeric/ublas/fwd.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

using namespace boost::numeric::ublas;
namespace Isis {
  /**
   * Constructor, creates a matrix scene widget.
   *
   * @param status Pointer to an existing status bar
   * @param showTools Whether or not create a matrix scene or matrix world view
   * @param internalizeToolBarsAndProgress Whether or not to show progress and status
   * @param directory The directory of the current project
   * @param parent QPointer to the parent widget
   */
  MatrixSceneWidget::MatrixSceneWidget(QStatusBar *status,
                                       bool showTools,
                                       bool internalizeToolBarsAndProgress,
                                       Directory *directory,
                                       QWidget *parent) : QWidget(parent) {
    m_directory = directory;

    m_graphicsScene = new MatrixGraphicsScene(this);
    m_graphicsScene->installEventFilter(this);

    m_graphicsView = new MatrixGraphicsView(m_graphicsScene, this);
    m_graphicsView->setScene(m_graphicsScene);
    m_graphicsView->setInteractive(true);

//     try {
//     m_matrixOptions = new MatrixOptions(
//         m_directory->project()->lastNotUndoneWorkOrder()->correlationMatrix(), this);
//     connect(m_matrixOptions, SIGNAL( optionsUpdated() ),
//             this, SLOT( redrawElements() ) );
//     }
//     catch (IException &e) {
//       throw IException(e, IException::Unknown, "bad workorder", _FILEINFO_);
//     }

    // Draw Initial Matrix
//     try {
//     drawElements( m_directory->project()->lastNotUndoneWorkOrder()->correlationMatrix() );
//     drawGrid( m_directory->project()->lastNotUndoneWorkOrder()->correlationMatrix() );
//     }
//     catch (IException &e) {
//       throw IException(e, IException::Unknown, "bad workorder", _FILEINFO_);
//     }
    
    m_graphicsView->setResizeAnchor(QGraphicsView::AnchorViewCenter);

    m_quickMapAction = NULL;

    m_outlineRect = NULL;

    m_progress = new ProgressBar;
    m_progress->setVisible(false);

    QGridLayout * sceneLayout = new QGridLayout;
    sceneLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(sceneLayout);

    // If we are making our own layout, we can create our own status area.
    if (!status && internalizeToolBarsAndProgress)
      status = new QStatusBar;

    if (showTools) {

      if (internalizeToolBarsAndProgress) {
        
        sceneLayout->addWidget(m_graphicsView, 1, 1, 1, 1);
        
        QHBoxLayout *horizontalStatusLayout = new QHBoxLayout;
        horizontalStatusLayout->addWidget(m_progress);
        horizontalStatusLayout->addStretch();
        horizontalStatusLayout->addWidget(status);

        sceneLayout->addLayout(horizontalStatusLayout, 2, 0, 1, 2);
      }
      else {
        sceneLayout->addWidget(m_graphicsView, 0, 0, 1, 1);
      }

      setWhatsThis("This is the matrix scene. You can interact with the "
                   "matrix elements shown here. ");

      getView()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
      getView()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

      getView()->enableResizeZooming(false);

      connect( getView()->horizontalScrollBar(), SIGNAL( valueChanged(int) ),
               this, SLOT( sendVisibleRectChanged() ) );
      connect( getView()->verticalScrollBar() , SIGNAL( valueChanged(int) ),
               this, SLOT( sendVisibleRectChanged() ) );
      connect( getView()->horizontalScrollBar(), SIGNAL( rangeChanged(int, int) ),
               this, SLOT( sendVisibleRectChanged() ) );
      connect( getView()->verticalScrollBar() , SIGNAL( rangeChanged(int, int) ),
               this, SLOT( sendVisibleRectChanged() ) );
    }
    else {
      sceneLayout->addWidget(m_graphicsView, 0, 0, 1, 1);

      getView()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      getView()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

      setWhatsThis("This is the matrix world view. The matrix will be "
          "shown here, but you cannot zoom in.");
    }
//     draw elements
  }


  /**
   * Default Destructor
   */
  MatrixSceneWidget::~MatrixSceneWidget() {
    m_outlineRect = NULL; // The scene will clean this up
  }


  /**
   * Accessor for the QGraphicsView
   *
   * @return @b MatrixGraphicsView* The matrix view.
   */
  MatrixGraphicsView *MatrixSceneWidget::getView() const {
    return m_graphicsView;
  }


  /**
   * Accessor for the QGraphicsScene
   *
   * @return @b MatrixGraphicsScene* The matrix scene.
   */
  QGraphicsScene *MatrixSceneWidget::getScene() const {
    return m_graphicsScene;
  }


  /**
   * This is called by MatrixGraphicsScene::contextMenuEvent.
   *
   * @param event The context menu event
   *
   * @return @b bool false if not handled, true if handled.
   */
  bool MatrixSceneWidget::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    bool handled = false;

    QList<QGraphicsItem *> selectedGraphicsItems = getScene()->selectedItems();
    // qDebug() << "number of selected items:" << selectedGraphicsItems.count();
//     QList<MatrixSceneItem *> selectedMatrixElementItems;
//     QList<MatrixElement*> selectedMatrixElements;
// 
//     foreach (QGraphicsItem *graphicsItem, selectedGraphicsItems) {
//       MatrixSceneItem *sceneMatrixElementItem = dynamic_cast<MatrixSceneItem *>(graphicsItem);
// 
//       if (!sceneMatrixElementItem) {
//         sceneMatrixElementItem = dynamic_cast<MatrixSceneItem *>( graphicsItem->parentItem() );
//       }
// 
//       if ( sceneMatrixElementItem && sceneMatrixElementItem->element() ) {
//         selectedMatrixElementItems.append(sceneMatrixElementItem);
//         selectedMatrixElements.append( sceneMatrixElementItem->element() );
//       }
//     }
// 
//     if ( selectedMatrixElementItems.count() ) {
//       QMenu menu;
// 
//       QAction *title = menu.addAction( tr("%L1 Selected MatrixElements").arg( selectedMatrixElements.count() ) );
//       title->setEnabled(false);
//       menu.addSeparator();
// 
//       Project *project = m_directory ? m_directory->project() : NULL;
// 
//       QList<QAction *> displayActs = selectedMatrixElements.supportedActions(project);
// 
//       if (m_directory) {
//         displayActs.append(NULL);
//         displayActs.append( m_directory->supportedActions( new QList<MatrixElement*>(selectedMatrixElements) ) );
//       }
// 
//       QAction *displayAct;
//       foreach(displayAct, displayActs) {
//         if (displayAct == NULL) {
//           menu.addSeparator();
//         }
//         else {
//           menu.addAction(displayAct);
//         }
//       }
// 
//       handled = true;
//       menu.exec( event->screenPos() );
//     }

    return handled;
  }


  /**
   * Accessor for the widget's progress bar
   *
   * @return @b QProgressBar* The progress bar
   */
  QProgressBar *MatrixSceneWidget::getProgress() {
    return m_progress;
  }



//   void MatrixSceneWidget::load(XmlStackedHandlerReader *xmlReader) {
//     xmlReader->pushContentHandler( new XmlHandler(this) );
//   }


  /**
   * Returns the bounding rectangle for the images
   *
   * @return @b QRectF Bounding rectangle for image elements
   */
  QRectF MatrixSceneWidget::elementsBoundingRect() const {
    QRectF boundingRect;

    if (m_outlineRect)
      boundingRect = boundingRect.united( m_outlineRect->boundingRect() );

    return boundingRect;
  }


  /**
   * Accessor for the directory
   *
   * @return @b Directory* The directory
   */
  Directory *MatrixSceneWidget::directory() const {
    return m_directory;
  }


  /**
   * Accessor for the view actions
   *
   * @return @b QList<QAction*> List of view actions
   */
  QList<QAction *> MatrixSceneWidget::getViewActions() {
    QList<QAction *> viewActs;

//     foreach(MatrixTool *tool, *m_tools) {
//       QList<QAction *> toolViewActs = tool->getViewActions();
//       viewActs.append(toolViewActs);
//     }

    return viewActs;
  }


  /**
   * Get a list of applicable actions for the elements in this scene.
   *
   * @param matrix Pointer to the CorrelationMatrix that belongs to the project
   * @return @b QList<QAction*> List of supported actions for the CorrelationMatrix elements
   */
  QList<QAction *> MatrixSceneWidget::supportedActions(CorrelationMatrix *matrix) {
    QList<QAction *> results;

    return results;
  }


  /**
   * This method refits the items in the graphics view.
   *
   */
//   void MatrixSceneWidget::refit() {
//     QRectF sceneRect = m_graphicsScene->itemsBoundingRect();
// 
//     if ( sceneRect.isEmpty() )
//       return;
// 
// //     double xPadding = sceneRect.width() * 0.10;
// //     double yPadding = sceneRect.height() * 0.10;
// // 
// //     sceneRect.adjust(-xPadding, -yPadding, xPadding, yPadding);
//     getView()->fitInView(sceneRect, Qt::KeepAspectRatio);
//   }


  /**
   * Update the visible bounding rectangle
   */
  void MatrixSceneWidget::sendVisibleRectChanged() {
    QPointF topLeft = getView()->mapToScene(0, 0);
    QPointF bottomRight =
        getView()->mapToScene( (int)getView()->width(), (int)getView()->height() );

    QRectF visibleRect(topLeft, bottomRight);
    emit visibleRectChanged(visibleRect);
  }


  /**
   * Pick the relevant actions given the type of event that occured in the scene.
   *
   * Mouse Events:
   * 
   * Press - Emit elementClicked signal to update the options dialog
   * Release - nolowerColorValue?
   * Double-Click - same as press
   * Move - nolowerColorValue
   * Enter - change label to match current item.
   * Leave - clear label
   * 
   * @param obj Pointer to the target QObject to watch events on
   * @param event The event to inspect
   *
   * @return @b bool True if the event is filter (stopping all future processing of the event) 
   */
  bool MatrixSceneWidget::eventFilter(QObject *obj, QEvent *event) {
    bool stopProcessingEvent = true;

    switch( event->type() ) {
      case QMouseEvent::GraphicsSceneMousePress: {
        if (m_graphicsScene->itemAt( ( (QGraphicsSceneMouseEvent *)event )->scenePos(),
                                        QTransform() ) ) {
          emit elementClicked(m_graphicsScene->itemAt(
                                ( (QGraphicsSceneMouseEvent *)event )->scenePos(),
                                   QTransform() )->toolTip() );
        }
        stopProcessingEvent = false;
        break;
      }

      case QMouseEvent::GraphicsSceneMouseDoubleClick:
        emit mouseDoubleClick(
            ((QGraphicsSceneMouseEvent *)event)->scenePos());
        m_graphicsView->fitInView(m_graphicsScene->itemsBoundingRect(), Qt::KeepAspectRatio);
        stopProcessingEvent = false;
        break;
      
      default:
        stopProcessingEvent = false;
        break;
    }

    return stopProcessingEvent;
  }


  /**
   * Redraws all the items in the view. Also makes sure to resize the view rectangle
   * to fit the newly drawn items.
   */
  void MatrixSceneWidget::redrawItems() {
  }


//   /**
//    * Refit scene and items when the widget size changes.
//    */
//   void MatrixSceneWidget::fitInView() {
//   }


  /**
   * Draw elements
   *
   * If element is drawn in black, something is wrong. The correlation value is out of range.
   * 
   * Need separate redraw method because we will already have the list of MatrixSceneItems.
   *
   * @param corrMatrix The matrix that belongs to the project.
   */
  void MatrixSceneWidget::drawElements(CorrelationMatrix corrMatrix) {
    int elementSize = 10;
    int startX = 20;
    int x = 20;
    int startY = 20;
    int y = 20;
    int yOffset = 0;
    QList<QGraphicsRectItem *> squares;
    QGraphicsRectItem *rectangle;
    QList<int> paramList;

    // This will change depending on the values of the elements
    QBrush fillBrush(Qt::blue);
    QPen outlinePen(Qt::black);
    outlinePen.setWidth(0);

    // Image and parameters of column
    QMapIterator<QString, QStringList> colIterator( *corrMatrix.imagesAndParameters() );
    // Image and parameters of row
    QMapIterator<QString, QStringList> rowIterator( *corrMatrix.imagesAndParameters() );
    
    foreach ( SparseBlockColumnMatrix blockColumn, *( corrMatrix.visibleBlocks() ) ) {
      QMapIterator<int, matrix<double>*> block(blockColumn);
      bool lastBlock = true;
      block.toBack(); // moves iterator to AFTER the last item
      colIterator.next();
      rowIterator = colIterator;

      do {
        block.previous(); // go to last item
        rowIterator.previous();
        for (int row = 0; row < (int)block.value()->size1(); row++) {
          for (int column = 0; column < (int)block.value()->size2(); column++) {

            // The values of this matrix should always be between 1 and -1.
            QColor fillColor;
            if ( !m_matrixOptions->colorScheme() ) {
              double red = 0;
              double green = 0;
              double lowerColorValue = fabs( ( *block.value() )(row, column) ) * 255.0;
              if (fabs( ( *block.value() )(row, column) ) < .5) {
                red = lowerColorValue * 2;
                green = 255;
              }
              else {
                green = 255 - ( (lowerColorValue - 127.5) * 2 );
                red = 255;
              }
               fillColor = QColor(red, green, 0);
            }

            bool draw = true;
            // Deal with diagonal.
            if (lastBlock) {
              if (column < row) {
                draw = false;
              }
              else if (column == row) {
                outlinePen.setColor(Qt::black);
                fillBrush.setColor(Qt::blue); // This will always be a single color
              }
              else {
                outlinePen.setColor(Qt::black);
                if ( m_matrixOptions->colorScheme() ) {
                  if ( fabs( ( *block.value() )(row, column) ) >=
                       m_matrixOptions->colorTolerance() ) {
                    fillBrush.setColor( m_matrixOptions->badCorrelationColor() );
                  }
                  else {
                    fillBrush.setColor( m_matrixOptions->goodCorrelationColor() );
                  }
                }
                else {
                  fillBrush.setColor(fillColor);
                }
              }
            }
            else {
              outlinePen.setColor(Qt::black);
                if ( m_matrixOptions->colorScheme() ) {
                  if ( fabs( ( *block.value() )(row, column) ) >=
                       m_matrixOptions->colorTolerance() ) {
                    fillBrush.setColor( m_matrixOptions->badCorrelationColor() );
                  }
                  else {
                    fillBrush.setColor( m_matrixOptions->goodCorrelationColor() );
                  }
                }
                else {
                  fillBrush.setColor(fillColor);
                }
            }

            // Should rectangle be a matrix element?
            if (draw) {
              rectangle = m_graphicsScene->addRect(x, y,                     // Start Position
                                                   elementSize, elementSize, // Rectangle Size
                                                   outlinePen, fillBrush);   // outline/fill options
              QString img1 = "Image 1       : " + colIterator.key() + "\n";
              QString param1 = "Parameter 1: " + colIterator.value().at(column) + "\n";
              QString img2 = "Image 2       : " + rowIterator.key() + "\n";
              QString param2 = "Parameter 2: " + rowIterator.value().at(row);
              QString toolTip = "Correlation  : " 
                  + QString::number( ( *block.value() )(row, column) )
                  + "\n" + img1 + param1 + img2 + param2;
              rectangle->setToolTip(toolTip);
            }
            x += elementSize;
          }
          x = startX;
          y += elementSize;
        }
        x = startX;
        // jump up by the number of rows in the previous block
        if ( block.hasPrevious() ) {
          yOffset += block.peekPrevious().value()->size1() * elementSize;
        }
        y = startY - yOffset;
        lastBlock = false;
      } while ( block.hasPrevious() );

      startX += block.value()->size2() * elementSize;
      startY += block.value()->size1() * elementSize;
      x = startX;
      y = startY;
      yOffset = 0;
    }
  }


  /**
   * Draw the lines that will show the columns and blocks for each image.
   *
   * @param corrMatrix The matrix that belongs to the project
   */
  void MatrixSceneWidget::drawGrid(CorrelationMatrix corrMatrix) {
    int startVX = 20;
    int startVY = 20;
    int startHX = 20;
    int startHY = 20;
    
    int elementSize = 10;
    
    QList<int> segments;
    int segmentLength = 0;

    QMapIterator<QString, QStringList> it( *corrMatrix.imagesAndParameters() );
    // get list of segment lengths1
    while ( it.hasNext() ) {
      it.next();
      int numOfParams = it.value().count();
      segmentLength += numOfParams * elementSize;
      segments.append(segmentLength);
    }

    segments.removeLast();
    
    QList<QGraphicsLineItem> lines;

    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(1);

    // Top of matrix, at this point segmentLength is the length of the longest side
    QGraphicsLineItem *hLine;
    hLine = new QGraphicsLineItem(startHX, startHY,
                                  startHX + segmentLength, startHY);
    hLine->setPen(pen);
    m_graphicsScene->addItem(hLine);

    // Right most side of matrix (edge)
    QGraphicsLineItem *vLine;
    vLine = new QGraphicsLineItem(startVX + segmentLength, startVY,
                                  startVX + segmentLength, startVY + segmentLength);
    vLine->setPen(pen);
    m_graphicsScene->addItem(vLine);

    int edge = startVX + segmentLength;
    int currentLength = segmentLength;
    int currentVX = startVX;

    //draw the rest of the grid
    foreach (int segment, segments) {
      currentVX = startVX + segment;
      currentLength = segmentLength - segment;

      vLine = new QGraphicsLineItem(currentVX, startVY,
                                    currentVX, startVY + segment);

      hLine = new QGraphicsLineItem(edge - currentLength, startHY + segment,
                                    edge, startHY + segment);

      vLine->setPen(pen);
      hLine->setPen(pen);
      m_graphicsScene->addItem(vLine);
      m_graphicsScene->addItem(hLine);
    }
  }


  /**
   * Redraw matrix when the focus is changed.
   */
  void MatrixSceneWidget::redrawElements() {
    drawElements( *m_matrixOptions->parentMatrix() );
  }


  /**
   * Change item colors when options are changed.
   *
   * @param colorScheme True if using tolerance. False if using gradient.
   */
//   void MatrixSceneWidget::repaintItems(bool colorScheme) {
//     if (colorScheme) {
//       // dont actually use this. just redo the colors?
//       drawElements( m_directory->project()->correlationMatrix() );
//     }
//     else {
//       foreach ( MatrixElement element, m_graphicsScene->items() ) {
//         if ( element.correlation() >= m_options->tolerance() ) {
//           element.setColor( m_options->badCorrelationColor() );
//         }
//         else {
//           element.setColor( m_options->goodCorrelationColor() );
//         }
//       }
//     }
//   }


  /**
   * Allows for changing the options on a CorrelationMatrix
   *
   * @param corrMat The matrix to set up options for
   */
  void MatrixSceneWidget::setUpOptions(CorrelationMatrix corrMat) {
    m_matrixOptions = new MatrixOptions(corrMat, this);
    connect(m_matrixOptions, SIGNAL( optionsUpdated() ),
            this, SLOT( redrawElements() ) );
  }
}
