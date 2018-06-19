#ifndef MosaicSceneWidget_H
#define MosaicSceneWidget_H

#include <QWidget>

#include "ImageList.h"
#include "Pvl.h"

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
  class ControlPoint;
  class Directory;
  class Image;
  class MosaicGraphicsView;
  class MosaicSceneItem;
  class MosaicTool;
  class ProgressBar;
  class Projection;
  class PvlGroup;
  class PvlObject;
  class ToolPad;

  /**
   * @brief This widget encompasses the entire mosaic scene
   *
   * This widget is a self-contained 2D footprint view using Qt's graphics scene/view framework.
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Stacy Alley
   *
   * @internal
   *   @history 2010-05-10 Christopher Austin - Added cnet connectivity
   *                           functionality and fixed a few design issues
   *   @history 2011-04-01 Steven Lambright - Separated this class from the
   *                           MosaicWidget class.
   *   @history 2011-05-10 Steven Lambright - Reduced unnecessary code, fixed
   *                           toolTips to work on everything (not just
   *                           cubes).
   *   @history 2011-05-17 Steven Lambright - More robust createInitialProj
   *   @history 2011-05-17 Steven Lambright - Target radii recalculated when
   *                           the user specifies a map file, if they
   *                           are missing.
   *   @history 2011-05-20 Steven Lambright - Improved error handling when
   *                           reprojecting.
   *   @history 2011-07-29 Steven Lambright - Z-ordering is now saved and
   *                           restored in the project files. references #275
   *   @history 2011-08-12 Steven Lambright - Added export options,
   *                           references #342
   *   @history 2011-08-29 Steven Lambright - Re-worded export file list option,
   *                           references #342
   *   @history 2011-09-27 Steven Lambright - Improved user documentation
   *   @history 2011-11-04 Steven Lambright - Added the zoom factor and
   *                           scroll bar position to the project file.
   *                           References #542.
   *   @history 2011-11-04 Steven Lambright - The mouse wheel events no longer
   *                           cause panning. The qt code for
   *                           QAbstractGraphicsView was looking at the event's
   *                           accepted state. This being fixed means the mouse
   *                           wheel can be used for zooming! Also added
   *                           getViewActions in order to allow the zooming key
   *                           shortcuts from the zoom tool to take effect when
   *                           the zoom tool wasn't active.
   *   @history 2012-06-20 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                           coding standards. References #972.
   *   @history 2012-07-26 Kimberly Oyama - Updated the help documentation for
   *                           the grid tool to include tips for the options
   *                           dialog and the new 'Auto Grid' functionality.
   *                           References #604.
   *   @history 2012-08-27 Tracie Sucharski - This widget now handles the creation of its own
   *                           tools to the toolbars.  Because this widget is used by qmos to create
   *                           both the world scene and mosaic scene, the bool showTools was
   *                           added to the constructor so that toolbars are not shown in the
   *                           world scene.
   *   @history 2012-09-12 Steven Lambright - Added xml save/load capabilities.
   *   @history 2012-09-17 Steven Lambright - Added very basic right-click on multiple images
   *                           capabilities. Increased performance.
   *   @history 2012-10-02 Stuart Sides and Steven Lambright - Added supportedActions() and updated
   *                           fitInView() to work when called by an action with an ImageList for
   *                           data.
   *   @history 2012-10-03 Steven Lambright - Removed createReferenceFootprint() - this was dead
   *                           code that the grid tool handles now.
   *   @history 2012-10-11 Debbie A. Cook, Updated to use new Target class.  References Mantis tickets 
   *                           #775 and #1114.
   *   @history 2012-10-19 Steven Lambright and Stuart Sides - Added moveUpOne(),
   *                           moveDownOne(), moveToTop(), and moveToBottom() methods with new,
   *                           more abstracted arguments. Added moveZ(). Improved fitInView()
   *                           capabilities. Added supportedActions().
   *   @history 2013-01-31 Steven Lambright - Fixed a problem caused by #1312 - when the minimum
   *                           longitude wasn't defined in the map file, the one generated by qmos
   *                           was invalid. Fixes #1406.
   *   @history 2012-12-21 Steven Lambright - Renamed askNewProjection() to
   *                           configProjectionParameters() and upgraded it's functionality to view
   *                           and edit the current projection. Fixes #1034.
   *   @history 2013-12-24 Tracie Sucharski - Initialized bool member variables,
   *                           m_queuedSelectionChanged and m_shouldRequestSelectionChanged.
   *                           Removed unused private member, m_projectionFootprint.  The
   *                           uninitialized values were causing the qmos selection tool to not
   *                           work properly.  Fixes #1742.
   *   @history 2014-07-18 Kimberly Oyama and Tracie Sucharski - Added selectedCubes() and other
   *                           control net functionality for IPCE.
   *   @history 2016-04-22 Jeannie Backer - Added label parameter to setProjection(mapGroup, label).
   *                           The default value for label is an empty Pvl. Also, modified
   *                           addImage() to pass the label from the cube of the image to be added
   *                           into setProjection(mapGroup, label). This was done to be able to call
   *                           TProjection::TargetRadii(label,mapGroup), which will
   *                           in turn will now call Target::radiiGroup(targetName), if needed. 
   *                           References #3892
   *   @history 2016-08-28 Kelvin Rodriguez - Added using QWidget::contextMenuEvent to avoid
   *                           hidden virtual function warnings in clang. Part of porting to OS X 10.11.
   *   @history 2016-09-14 Ian Humphrey - Replaced deprecated QPixmap::grabWidget with
   *                           QWidget::grab. Fixes #4304.
   *   @history 2016-04-22 Jeannie Backer - Added label parameter to setProjection(mapGroup, label).
   *                           The default value for label is an empty Pvl. Also, modified
   *                           addImage() to pass the label from the cube of the image to be added
   *                           into setProjection(mapGroup, label). This was done to be able to call
   *                           TProjection::TargetRadii(label,mapGroup), which will
   *                           in turn will now call Target::radiiGroup(targetName), if needed. 
   *                           References #3892
   *   @history 2016-08-02 Tracie Sucharski - Added public method to remove an Image from scene. 
   *   @history 2016-09-14 Tracie Sucharski - Added signals for mouse clicks for modifying, deleting
   *                           and creating control points.  These are passed on to Footprint2DView
   *                           signals, then on to Directory slots.
   *   @history 2016-11-07 Ian Humphrey - Restored the startElement() functionality so footprints
   *                           can be correctly loaded from an XML storing the MosaicScenWidget's
   *                           state. Fixes #4486.
   *   @history 2017-07-27 Makayla Shepherd - Fixed a segfault that occurred when closing a cube
   *                           footprint. Fixes #5050.
   *   @history 2017-08-02 Tracie Sucharski - Added cnetModified signal to indicate that the control
   *                           point graphics items need to be rebuilt since either a control point
   *                           was deleted or added to the control net.  Renamed deleteControlPoint
   *                           signal to controlPointDeleted.  Removed some unneeded connections.
   *                           Fixes #5007, #5008.
   *   @history 2018-05-14 Tracie Sucharski - Change the xml tag from footprint2DView to
   *                           mosaicScene. Reference #5422.
   *  
   */
  class MosaicSceneWidget : public QWidget {
      Q_OBJECT

    public:
      MosaicSceneWidget(QStatusBar *status,
                        bool showTools, bool internalizeToolBarsAndProgress,
                        Directory *directory, QWidget *parent = 0);
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
      void addTo(ToolPad *toolPad);
      void addToPermanent(QToolBar *toolBar);
      void addTo(QToolBar *toolBar);

      using QWidget::contextMenuEvent;
      bool contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

      MosaicSceneItem *cubeToMosaic(Image *);

      bool cubesSelectable() const {
        return m_cubesSelectable;
      }

      void enableRubberBand(bool);
      bool blockSelectionChange(bool);

      bool userHasTools() const {
        return m_userToolControl;
      }

      QProgressBar *getProgress();
      PvlObject toPvl() const;
      void fromPvl(const PvlObject &);
      void load(XmlStackedHandlerReader *xmlReader);
      void save(QXmlStreamWriter &stream, Project *project, FileName newProjectRoot) const;

//    QPointF currentLatLonPosition();
      QRectF cubesBoundingRect() const;
      QStringList cubeFileNames();
      Directory *directory() const;

      ImageList images();
      ImageList selectedImages();

      QList<QAction *> getExportActions();
      QList<QAction *> getViewActions();
      QList<QAction *> supportedActions(ImageList *);

      double moveDownOne(MosaicSceneItem *);
      double moveDownOne(Image *);
      QList<double> moveDownOne(ImageList *);

      double moveToBottom(MosaicSceneItem *);
      double moveToBottom(Image *);
      QList<double> moveToBottom(ImageList *);

      double moveUpOne(MosaicSceneItem *);
      double moveUpOne(Image *);
      QList<double> moveUpOne(ImageList *);

      double moveToTop(MosaicSceneItem *);
      double moveToTop(Image *);
      QList<double> moveToTop(ImageList *);

      double moveZ(MosaicSceneItem *sceneItem, double newZ, bool newZValueMightExist = true);
      double moveZ(Image *image, double newZ, bool newZValueMightExist = true);

      /**
       * Return an empty list of actions for unknown data types
       */
      template <typename DataType>
      QList<QAction *> supportedActions(DataType) {
        return QList<QAction *>();
      }

      bool isControlNetToolActive();

      static QWidget *getControlNetHelp(QWidget *cnetToolContainer = NULL);
      static QWidget *getGridHelp(QWidget *gridToolContainer = NULL);
      static QWidget *getLongHelp(QWidget *mosaicSceneContainer = NULL);
      static QWidget *getMapHelp(QWidget *mapContainer = NULL);
      static QWidget *getPreviewHelp(QWidget *worldViewContainer = NULL);

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

      void queueSelectionChanged();

      void modifyControlPoint(ControlPoint *controlPoint);
      void deleteControlPoint(ControlPoint *controlPoint);
      void createControlPoint(double latitude, double longitude);

      void controlPointSelected(ControlPoint *);

      void cnetModified();
      void controlPointChanged(QString pointId);
      void controlPointDeleted();
      
      void mosCubeClosed(Image *);

    public slots:
      void addImages(ImageList);
      void removeImages(ImageList);
      void refit();
      void setCubesSelectable(bool);
      void setProjection(Projection *);
      void setOutlineRect(QRectF);

    private slots:
      void exportView();
      void saveList();

      void removeMosItem(QObject *);

      void moveDownOne();
      void moveToBottom();
      void moveUpOne();
      void moveToTop();
      void fitInView();

      void onSelectionChanged();
      void onQueuedSelectionChanged();

      void configProjectionParameters();
      void quickConfigProjectionParameters();
      void sendVisibleRectChanged();

    protected:
      virtual bool eventFilter(QObject *obj, QEvent *ev);

    private:
      void setProjection(const PvlGroup &mappingGroup, Pvl label=Pvl());
      MosaicSceneItem *addImage(Image *image);
      void reprojectItems();
      double maximumZ();
      double minimumZ();
      void recalcSceneRect();

      MosaicSceneItem *getNextItem(MosaicSceneItem *item, bool up);

      PvlGroup createInitialProjection(Image *cube);

      MosaicSceneItem *cubeToMosaic(DisplayProperties *props);

      static bool zOrderGreaterThan(MosaicSceneItem *first,
                                    MosaicSceneItem *second);

    private:
      /**
       * @author 2012-09-?? Steven Lambright
       *
       * @internal
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(MosaicSceneWidget *scene);
          ~XmlHandler();

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);
          virtual bool characters(const QString &ch);
          virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                  const QString &qName);

        private:
          Q_DISABLE_COPY(XmlHandler);

          QString m_characterData;
          MosaicSceneWidget *m_scene;

          int m_scrollBarXValue;
          int m_scrollBarYValue;

          ImageList *m_imagesToAdd;
          QList<double> m_imageZValues;
      };

    private:
      Directory *m_directory;

      bool m_cubesSelectable;
      bool m_customRubberBandEnabled;
      QRubberBand *m_customRubberBand;
      QPoint *m_rubberBandOrigin;
      QGraphicsScene *m_graphicsScene; //!< The graphics scene that makes up this widget.
      MosaicGraphicsView *m_graphicsView; //!< The graphics view
      Projection *m_projection; //!< The current projection type.
      QList<MosaicSceneItem *> *m_mosaicSceneItems;
      QMap<DisplayProperties *, MosaicSceneItem *> m_displayPropsToMosaicSceneItemMap;
      QGraphicsRectItem *m_outlineRect;

      QToolButton *m_mapButton;
      QAction *m_quickMapAction;

      QList<MosaicTool *> *m_tools;

      ToolPad *m_toolpad;

      QToolBar *m_permToolbar;
      QToolBar *m_activeToolbar;

      ProgressBar *m_progress;

      QHash<QString, double> *m_projectImageZOrders;

      bool m_blockingSelectionChanged;
      bool m_userToolControl;
      bool m_ownProjection;
      bool m_queuedSelectionChanged;
      bool m_shouldRequeueSelectionChanged;

      double m_currentMinimumFootprintZ;
      double m_currentMaximumFootprintZ;
      
      PvlObject *m_projectViewTransform;
  };
}

#endif

