#ifndef MatrixSceneWidget_H
#define MatrixSceneWidget_H

#include <QWidget>

#include "XmlStackedHandler.h"

template <typename A> class QList;
class QGraphicsPolygonItem;
class QGraphicsRectItem;
class QGraphicsScene;
class QGraphicsSceneContextMenuEvent;
class QMenu;
class QProgressBar;
class QRubberBand;
class QStatusBar;
class QToolBar;
class QToolButton;

namespace Isis {
  class CorrelationMatrix;
  class Directory;
  class DisplayProperties;
  class FileName;
  class MatrixDisplayTool;
  class MatrixGraphicsView;
  class MatrixOptions;
  class ProgressBar;
  class Project;
  class PvlGroup;
  class PvlObject;
  class ToolPad;

  /**
   * @brief This widget encompasses the entire matrixDisplay scene
   *
   * This widget is a self-contained view of the correlation matrix resulting from a bundle adjust.
   * It uses Qt's graphics scene/view framework. This widget holds the graphics scene, view, and
   * items. It will also hold the options dialog for the matrix.
   *
   * @ingroup Visualization Tools
   *
   * @author 2014-05-10 Kimberly Oyama
   *
   * @internal
   *   @history 2014-07-14 Kimberly Oyama - Original Version
   *   @history 2014-07-21 Kimberly Oyama - Added matrix options dialog. Color options are
   *                           functional.
   *   @history 2016-06-27 Ian Humphrey - Updated documentation and coding standards. Fixes #4006.
   */
  class MatrixSceneWidget : public QWidget {
      Q_OBJECT

    public:
      MatrixSceneWidget(QStatusBar *status,
                        bool showTools,
                        bool internalizeToolBarsAndProgress,
                        Directory *directory,
                        QWidget *parent = 0);
      virtual ~MatrixSceneWidget();

      MatrixGraphicsView *getView() const;
      QGraphicsScene *getScene() const;

      bool contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

      void drawGrid(CorrelationMatrix corrMatrix);
      void drawElements(CorrelationMatrix corrMatrix);

      QProgressBar *getProgress();
      PvlObject toPvl() const;
      void fromPvl(const PvlObject &);

      QRectF elementsBoundingRect() const;
      Directory *directory() const;
      QList<QAction *> getViewActions();
      QList<QAction *> supportedActions(CorrelationMatrix *matrix);
      void redrawItems();
//       void refit();

//       void repaintItems(bool colorScheme);
      void setUpOptions(CorrelationMatrix corrMat);

      /**
       * Return an empty list of actions for unknown data types
       *
       * @param DataType The type of data we want to get actions for
       *
       * @return @b QList<QAction*> Empty list of actions (currently)
       */
      template <typename DataType>
      QList<QAction *> supportedActions(DataType) {
        return QList<QAction *>();
      }

    signals:
// add roll over?
      // Not all these signals are used, descriptions show possible intended use
      void mouseEnter(); //!< Emitted when mouse enters widget focus
      void mouseMove(QPointF); //!< Emitted when mouse moves in widget
      void mouseLeave(); //!< Emitted when mouse leaves widget focus
      void mouseDoubleClick(QPointF); //!< Emitted when handling a mouse double-click
      void mouseButtonPress(QPointF, Qt::MouseButton s); //!< Emitted when mouse is pressed
      void mouseButtonRelease(QPointF, Qt::MouseButton s); //!< Emitted when mouse button released
      void mouseWheel(QPointF, int delta); //!< Emitted when mouse wheel is activated
      //! Emitted when rubber band selection is complete
      void rubberBandComplete(QRectF r, Qt::MouseButton s);
      void visibleRectChanged(QRectF); //!< Emitted when the visible rectangle is changed
      void elementsChanged(); //!< Emitted when an element of the widget is changed
      void queueSelectionChanged(); //!< Emitted when selection is changed

      void elementClicked(QString); //!< Emitted when an element of the widget is clicked

    public slots:
      void redrawElements();

    protected:
      virtual bool eventFilter(QObject *obj, QEvent *ev);
      
    private slots:

//       void fitInView();

      void sendVisibleRectChanged();
      
    private:
      // methods
      QList<double> getSelectedElements() const;

      // member variables
      Directory *m_directory; //!< The directory of the project
      
      QGraphicsScene *m_graphicsScene; //!< The graphics scene holds the scene items.
      MatrixGraphicsView *m_graphicsView; //!< The graphics view that displays the scene

      QGraphicsRectItem *m_outlineRect; //!< Rectangle outlining the area where the images go.

      QAction *m_quickMapAction; //!< ??? (always NULL)

      ProgressBar *m_progress; //!< Progress bar

      MatrixOptions *m_matrixOptions; //!< Options for the correlation matrix
  };
}

#endif

