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
   */
  class MosaicSceneWidget : public QWidget {
      Q_OBJECT

    public:
      MosaicSceneWidget(QStatusBar *status, QWidget *parent = 0);
      virtual ~MosaicSceneWidget();

      MosaicGraphicsView *getView() const {
        return p_graphicsView;
      }

      QGraphicsScene *getScene() const {
        return p_graphicsScene;
      }

      Projection *getProjection() const {
        return p_projection;
      }

      QList<MosaicSceneItem *> allMosaicSceneItems() {
        return *p_mosaicSceneItems;
      }

      void addTo(QMenu *menu);
      void addTo(Qisis::ToolPad *toolPad);
      void addToPermanent(QToolBar *toolBar);
      void addTo(QToolBar *toolBar);

      bool cubesSelectable() const {
        return p_cubesSelectable;
      }

      void enableRubberBand(bool);
      void blockSelectionChange(bool);

      bool userHasTools() const {
        return p_userToolControl;
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
      void setLevelOfDetail(int);

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

      bool p_cubesSelectable;
      bool p_customRubberBandEnabled;
      QRubberBand *p_customRubberBand;
      QPoint *p_rubberBandOrigin;
      QGraphicsScene *p_graphicsScene; //!< The graphics scene that makes up this widget.
      MosaicGraphicsView *p_graphicsView; //!< The graphics view
      Projection *p_projection; //!< The current projection type.
      QGraphicsPolygonItem *p_projectionFootprint;
      QList<MosaicSceneItem *> *p_mosaicSceneItems;
      QGraphicsRectItem *p_outlineRect;

      QAction *p_mapAction;

      QList<MosaicTool *> *p_tools;
      ProgressBar *p_progress;

      bool p_userToolControl;
      bool p_ownProjection;
  };
}

#endif

