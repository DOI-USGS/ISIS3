#ifndef MosaicSceneWidget_H
#define MosaicSceneWidget_H

#include <QWidget>

template <typename A> class QList;
class QGraphicsPolygonItem;
class QGraphicsRectItem;
class QGraphicsScene;
class QMenu;
class QProgressBar;
class QRubberBand;
class QStatusBar;
class QToolBar;

namespace Qisis {
  class ToolPad;
}

namespace Isis {
  class CubeDisplayProperties;
  class MosaicGraphicsView;
  class MosaicSceneItem;
  class MosaicTool;
  class ProgressBar;
  class Projection;
  class PvlObject;

  /**
   * @brief This widget encompasses the entire mosaic scene and is
   *        what you want to use from an application's point of view
   *
   * @ingroup Visualization Tools
   *
   * @author Stacy Alley
   *
   * @internal
   *  @history 2010-05-10 Christopher Austin - Added cnet connectivity
   *                                functionality and fixed a few design issues
   *  @history 2011-04-01 Steven Lambright - Separated this class from the
   *                                MosaicWidget class.
   *  @history 2011-05-10 Steven Lambright - Reduced unnecessary code, fixed
   *                                toolTips to work on everything (not just
   *                                cubes). 
   *  @history 2011-05-17 Steven Lambright - More robust createInitialProj 
   */
  class MosaicSceneWidget : public QWidget {
      Q_OBJECT

    public:
      MosaicSceneWidget(QStatusBar *status, QWidget *parent = 0);
      virtual ~MosaicSceneWidget();

      MosaicGraphicsView *getView() const {
        return m_graphicsView;
      }

      QGraphicsScene *getScene() const {
        return m_graphicsScene;
      }

      Projection *getProjection() const {
        return m_projection;
      }

      QList<MosaicSceneItem *> allMosaicSceneItems() {
        return *m_mosaicSceneItems;
      }

      void addTo(QMenu *menu);
      void addTo(Qisis::ToolPad *toolPad);
      void addToPermanent(QToolBar *toolBar);
      void addTo(QToolBar *toolBar);

      bool cubesSelectable() const {
        return m_cubesSelectable;
      }

      void enableRubberBand(bool);
      void blockSelectionChange(bool);

      bool userHasTools() const {
        return m_userToolControl;
      }

      QProgressBar *getProgress();
      PvlObject toPvl() const;
      void fromPvl(PvlObject);

      QRectF cubesBoundingRect() const;
      QStringList cubeFilenames();
      QList<CubeDisplayProperties *> cubeDisplays();

    signals:
      void mouseEnter();
      void mouseMove(QPointF);
      void mouseLeave();
      void mouseDoubleClick(QPointF);
      void mouseButtonPress(QPointF, Qt::MouseButton s);
      void mouseButtonRelease(QPointF, Qt::MouseButton s);
      void mouseWheel(QPointF, int delta);
      void projectionChanged(Projection *);
      void rubberBandComplete(QRectF r, Qt::MouseButton s);
      void visibleRectChanged(QRectF);

      void cubesChanged();

    public slots:
      void addCubes(QList<CubeDisplayProperties *>);
      void refit();
      void setCubesSelectable(bool);
      void setProjection(Projection *);
      void setOutlineRect(QRectF);

    private slots:
      void removeMosItem(QObject *);

      void moveDownOne();
      void moveToBottom();
      void moveUpOne();
      void moveToTop();
      void fitInView();

      void onSelectionChanged();

      void askNewProjection();
      void sendVisibleRectChanged();

    protected:
      virtual bool eventFilter(QObject *obj, QEvent *ev);

    private:
      MosaicSceneItem *addCube(CubeDisplayProperties *);
      void createReferenceFootprint();
      void reprojectItems();
      qreal maximumZ();
      qreal minimumZ();
      void recalcSceneRect();

      MosaicSceneItem *getNextItem(MosaicSceneItem *item, bool up);

      Projection *createInitialProjection(CubeDisplayProperties *cube);

      MosaicSceneItem *cubeToMosaic(CubeDisplayProperties *);

      QList<CubeDisplayProperties *> getSelectedCubes() const;

      bool m_cubesSelectable;
      bool m_customRubberBandEnabled;
      QRubberBand *m_customRubberBand;
      QPoint *m_rubberBandOrigin;
      QGraphicsScene *m_graphicsScene; //!< The graphics scene that makes up this widget.
      MosaicGraphicsView *m_graphicsView; //!< The graphics view
      Projection *m_projection; //!< The current projection type.
      QGraphicsPolygonItem *m_projectionFootprint;
      QList<MosaicSceneItem *> *m_mosaicSceneItems;
      QGraphicsRectItem *m_outlineRect;

      QAction *m_mapAction;

      QList<MosaicTool *> *m_tools;
      ProgressBar *m_progress;

      bool m_userToolControl;
      bool m_ownProjection;
  };
}

#endif

