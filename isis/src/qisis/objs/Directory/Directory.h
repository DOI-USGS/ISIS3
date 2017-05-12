#ifndef Directory_H
#define Directory_H
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

#include <QObject>
#include <QPointer>
#include <QString>
#include <QtDebug>


#include "GuiCameraList.h"
#include "ImageList.h"
#include "MosaicSceneWidget.h"
#include "TargetBodyList.h"
#include "WorkOrder.h"

class QAction;
class QDockWidget;
class QMenuBar;
class QProgressBar;
class QSplitter;
class QTabWidget;

namespace Isis {
  class BundleObservation;
  class BundleObservationView;
  class ChipViewportsWidget;
  class CnetEditorWidget;
  class ControlNet;
  class ControlPointEditView;
  class CubeDnView;
  class FileItem;
  class Footprint2DView;
  class HistoryTreeWidget;
  class ImageFileListWidget;
  class MatrixSceneWidget;
  class MosaicSceneWidget;
  class Project;
  class ProjectItem;
  class ProjectItemModel;
  class ProjectItemTreeView;
  class SensorInfoWidget;
  class TargetBody;
  class TargetInfoWidget;
  class WarningTreeWidget;
  class WorkOrder;
  class Workspace;

  /**
   *
   * @author 2012-??-?? ???
   *
   * @internal
   *   @history 2012-07-30 Steven Lambright - The save action now has enabling/disabling of state
   *                           functional (as long as there are work orders in the undo stack).
   *   @history 2012-08-28 Tracie Sucharski - Instead of this class adding tabs to a TabWidget, it
   *                           now emits a signal which is connected to cnetSuiteMainWindow to
   *                           create a new dock widget.  This class no longer needs the
   *                           viewContainer since it is not adding tabs.
   *   @history 2012-09-12 Steven Lambright - Added xml save/load capabilities, removed dead code
   *                           relating to having only one image list (now we have N image lists).
   *   @history 2012-09-19 Steven Lambright - Re-implemented workOrders(ImageList *) into a generic
   *                           templated version. Added m_workOrders and createWorkOrder().
   *   @history 2012-10-02 Stuart Sides and Steven Lambright - Renamed workOrders() to
   *                           supportedActions(). This method now asks the footprint views for
   *                           their supported actions in addition to the known work orders. Added
   *                           sorting/smart arranging of the actions that come from the footprint
   *                           views.
   *   @history 2012-10-03 Steven Lambright - Added 'All' option generation in restructureActions()
   *   @history 2014-07-14 Kimberly Oyama - Updated to better meet programming standards. Added
   *                           support for correlation matrix.
   *   @history 2015-10-05 Jeffrey Covington - Added a ProjectItemModel and the
   *                           addProjectItemTreeView() method to start
   *                           supporting Qt's model-view framework.
   *   @history 2016-01-04 Jeffrey Covington - Added support for CubeDnView and
   *                           Footprint2DView, replacing old classes.
   *   @history 2016-06-17 Tyler Wilson - Added documentation for member functions/variables.
   *   @history 2016-07-06 Tracie Sucharski - Added ImportShapesWorkOrder, changed ControlNetEditor
   *                           to ControlPointEditView.
   *   @history 2016-08-02 Tracie Sucharski - Added RemoveImagesWorkOrder.
   *   @history 2016-09-14 Tracie Sucharski - Added slots for mouse clicks on Footprint2DView and
   *                           CubeDnViews for modifying, deleting and creating control points.
   *   @history 2016-11-07 Ian Humphrey - Restored saving and loading Footprint2DViews when saving
   *                           and opening a project (modified save() and startElement()).
   *                           Fixes #4486.
   *   @history 2016-11-10 Tracie Sucharski - Added functionality to save/restore CubeDnViews.
   *   @history 2016-12-21 Tracie Sucharski - Added QObject parameter to
   *                           cleanupFootprint2DViewWidgets.  All footprint views were being
   *                           destroyed rather than simply the view which was closed. TODO: This
   *                           also needs to be fixed for all other cleanup(View) methods.
   *   @history 2017-02-08 Tracie Sucharski - Implemented quick&dirty auto-save for active control
   *                           net.
   *   @history 2017-02-23 Tracie Sucharski - Removed populateMainMenu method.  It became obsolete
   *                           during changes Jeffrey Covington made on 1-4-2016, rev 6511.
   *   @history 2017-02-28 Tracie Sucharski - Added ability to set the colors for the ControlPoint
   *                           display on views which show ControlPoints such as CubeDnView and
   *                           Footprint2DView.  This is done as an application setting so that all
   *                           views use the same colors.  Directory stores the colors so that any
   *                           registered view can get the current colors.
   *   @history 2017-04-17 Tracie Sucharski - Added connection between model's projectNameEdited,
   *                           initiated by double-clicking the project name on the ProjectTreeView
   *                           and Directory's slot, initiateRenameProjectWorkOrder.  Fixes #2295.
   *   @history 2017-04-17 Ian Humphrey - Modified how ExportControlNet, ExportImages, and
   *                           Jigsaw WorkOrder's are added to the main window menu. These are
   *                           disabled by default, and connections are setup to listen for when
   *                           cnets are added, when images are added, and when both an active
   *                           cnet and image list have been set. Fixes #4749.
   *   @history 2017-04-25 Ian Humphrey - Modified initializeActions() so that the jigsaw work
   *                           order is enabled whenever there are both images and cnets in the
   *                           project. Otherwise, it is disabled until then. Fixes #4819.
   *   @history 2017-05-03 Tracie Sucharski - Added methods and member variables for the
   *                           BundleObservationView.  Fixes #4839. Fixes #4840.
   */
  class Directory : public QObject {
    Q_OBJECT
    public:
      explicit Directory(QObject *parent = 0);
      ~Directory();

      void setHistoryContainer(QDockWidget *historyContainer);
      void setWarningContainer(QDockWidget *warningContainer);
      void setRecentProjectsList(QStringList recentProjects);
      QStringList recentProjectsList();

      BundleObservationView *addBundleObservationView(FileItemQsp fileItem);
      CnetEditorWidget *addCnetEditorView(Control *network);
      CubeDnView *addCubeDnView();
      Footprint2DView *addFootprint2DView();
      MatrixSceneWidget *addMatrixView();
      TargetInfoWidget *addTargetInfoView(TargetBodyQsp target);
      SensorInfoWidget *addSensorInfoView(GuiCameraQsp camera);
      ImageFileListWidget *addImageFileListView();
      ControlPointEditView *addControlPointEditView();


      ProjectItemTreeView *addProjectItemTreeView();

      ProjectItemModel *model();

      Project *project() const;

      QList<QAction *> fileMenuActions();
      QList<QAction *> projectMenuActions();
      QList<QAction *> editMenuActions();
      QList<QAction *> viewMenuActions();
      QList<QAction *> settingsMenuActions();
      QList<QAction *> helpMenuActions();

      QList<QAction *> permToolBarActions();
      QList<QAction *> activeToolBarActions();
      QList<QAction *> toolPadActions();

      QList<BundleObservationView *> bundleObservationViews();
      QList<CnetEditorWidget *> cnetEditorViews();
      QList<CubeDnView *> cubeDnViews();
      QList<Footprint2DView *> footprint2DViews();
      QList<MatrixSceneWidget *> matrixViews();
      QList<SensorInfoWidget *> sensorInfoViews();
      QList<TargetInfoWidget *> targetInfoViews();
      QList<ImageFileListWidget *> imageFileListViews();
      QList<QProgressBar *> progressBars();
      ControlPointEditView *controlPointEditView();
      ChipViewportsWidget *controlPointChipViewports();



      /**
       * @brief Returns a list of supported actions for a WorkOrder
       * @param data The WorkOrder type we are using.
       * @return @b QList<QAction *> A list of supported actions.
       */
      template <typename DataType>
      QList<QAction *> supportedActions(DataType data) {
        QList<QAction *> results;

        //QList< QPair< QString, QList<QAction *> > > actionPairings;

        //foreach (MosaicSceneWidget *footprint2DView, m_footprint2DViewWidgets) {
        //  actionPairings.append(
        //      qMakePair(footprint2DView->windowTitle(), footprint2DView->supportedActions(data)));
        //}

        //results.append(restructureActions(actionPairings));

        //if (!results.isEmpty()) {
        //  results.append(NULL);
        //}
//      qDebug()<<"Directory.h::supportedActions  #workorders = "<<m_workOrders.size();
        foreach (WorkOrder *workOrder, m_workOrders) {
          if (workOrder->isExecutable(data)) {
            WorkOrder *clone = workOrder->clone();
            clone->setData(data);
            results.append(clone);
          }
        }

        return results;
      }

      void showWarning(QString text);


      /**
       * @brief Shows warning text for a Widget.
       * @param text The warning text.
       * @param Data The Widget object we are passing the warning to.
       */
      template <typename Data>
      void showWarning(QString text, Data data) {
        //m_warningTreeWidget->showWarning(text, data);
      }

      QWidget *warningWidget();

      QAction *redoAction();
      QAction *undoAction();

      void load(XmlStackedHandlerReader *xmlReader);
      void save(QXmlStreamWriter &stream, FileName newProjectRoot) const;

    signals:
      void newWidgetAvailable(QWidget *newWidget);

      void controlPointAdded(QString newPointId);

    public slots:
      void cleanupBundleObservationViews();
      void cleanupCnetEditorViewWidgets();
      void cleanupCubeDnViewWidgets();
      void cleanupFileListWidgets();
      void cleanupFootprint2DViewWidgets(QObject *);
      void cleanupControlPointEditViewWidget();
      void cleanupMatrixViewWidgets();
      void cleanupSensorInfoWidgets();
      void cleanupTargetInfoWidgets();
      //void imagesAddedToProject(ImageList *images);
      void updateControlNetEditConnections();

      // TODO temporary slot until autosave is implemented
      void makeBackupActiveControl();

      //  Slots in response to mouse clicks on CubeDnView (IpceTool) and
      //    Footprint2DView (MosaicControlNetTool)
      void modifyControlPoint(ControlPoint *controlPoint);
      void deleteControlPoint(ControlPoint *controlPoint);
      void createControlPoint(double latitude, double longitude, Cube *cube = 0,
                              bool isGroundSource = false);

      void updateRecentProjects(Project *project);

    private slots:
      void initiateRenameProjectWorkOrder(QString projectName);

    private:
      /**
       * @author 2012-08-?? Steven Lambright
       *
       * @internal
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(Directory *directory);
          ~XmlHandler();

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);

        private:
          Q_DISABLE_COPY(XmlHandler);

          Directory *m_directory;  //!< Pointer to a Directory which is set by the XmlHandler class.
      };

    private:
      Directory(const Directory &other);
      Directory &operator=(const Directory &rhs);

      /**
       * @brief Create a work order, append it to m_workOrders, and return it.
       *
       * Example:
       *   createWorkOrder<ImageFileListViewWorkOrder>();
       *   This will create a new ImageFileListViewWorkOrder and append it to m_workOrders.
       * @return @b A pointer to the WorkOrder created by this function.
       */
      template <typename WorkOrderType>
      WorkOrderType *createWorkOrder() {
        WorkOrderType *newWorkOrder = new WorkOrderType(m_project);
        m_workOrders.append(newWorkOrder);
        return newWorkOrder;
      }

      static QList<QAction *> restructureActions(QList< QPair< QString, QList<QAction *> > >);
      static bool actionTextLessThan(QAction *lhs, QAction *rhs);

      void initializeActions();

      QPointer<ProjectItemModel> m_projectItemModel; //!< Pointer to the ProjectItemModel.


      QPointer<HistoryTreeWidget> m_historyTreeWidget;  //!< Pointer to the HistoryTreeWidget.
      QPointer<Project> m_project;                      //!< Pointer to the Project.
      QPointer<WarningTreeWidget> m_warningTreeWidget;  //!< Pointer to the WarningTreeWidget.

      //!< List of BundleObservationView
      QList< QPointer<BundleObservationView> > m_bundleObservationViews;
      QList< QPointer<CnetEditorWidget> > m_cnetEditorViewWidgets;  //!< List of CnetEditorWidgets
      QList< QPointer<CubeDnView> > m_cubeDnViewWidgets;  //!< List of CubeDnCiew obs
      QList< QPointer<ImageFileListWidget> > m_fileListWidgets;  //!< List of ImageFileListWidgets
      QList< QPointer<Footprint2DView> > m_footprint2DViewWidgets; //!< List of Footprint2DView objs
      QPointer <ControlPointEditView> m_controlPointEditViewWidget;
      QPointer <ChipViewportsWidget> m_chipViewports;
      QList< QPointer<MatrixSceneWidget> > m_matrixViewWidgets; //!< List of MatrixSceneWidgets
      QList< QPointer<SensorInfoWidget> > m_sensorInfoWidgets; //!< List of SensorInfoWidgets
      QList< QPointer<TargetInfoWidget> > m_targetInfoWidgets; //!< List of TargetInfoWidgets

      QList< QPointer<WorkOrder> > m_workOrders; //!< List of WorkOrders

      QStringList m_recentProjects;  //!< List of the names of recent projects

      // We only need to store the work orders that go into menus uniquely... all work orders
      //   (including these) should be stored in m_workOrders
      QPointer<WorkOrder> m_exportControlNetWorkOrder;  //!< The export ControlNetwork WorkOrder.
      QPointer<WorkOrder> m_exportImagesWorkOrder; //!< The export images WorkOrder.
      QPointer<WorkOrder> m_importControlNetWorkOrder; //!< The import ControlNetwork WorkOrder.
      QPointer<WorkOrder> m_importImagesWorkOrder; //!< The import images WorkOrder.
      QPointer<WorkOrder> m_importShapesWorkOrder; //!< The import shapes WorkOrder.
      QPointer<WorkOrder> m_openProjectWorkOrder; //!< The Open Project WorkOrder.
      QPointer<WorkOrder> m_saveProjectWorkOrder; //!< The Save Project WorkOrder.
      QPointer<WorkOrder> m_saveProjectAsWorkOrder; //!< The Save Project As WorkOrder.
      QPointer<WorkOrder> m_openRecentProjectWorkOrder; //!< The Open Recent Project WorkOrder.
      QPointer<WorkOrder> m_closeProjectWorkOrder; //!< The Close Project WorkOrder

      QPointer<WorkOrder> m_runJigsawWorkOrder; //!< The Run Jigsaw WorkOrder
      QPointer<WorkOrder> m_renameProjectWorkOrder; //!< The Rename Project WorkOrder

      QList<QAction *> m_fileMenuActions;  //!< List of file menu actions.
      QList<QAction *> m_projectMenuActions; //!< List of project menu actions.
      QList<QAction *> m_editMenuActions; //!< List of edit menu actions.
      QList<QAction *> m_viewMenuActions; //!< List of view menu actions.
      QList<QAction *> m_settingsMenuActions; //!< List of menu settings actions
      QList<QAction *> m_helpMenuActions; //!< List of help menu actions

      QList<QAction *> m_permToolBarActions; //!< List of perm ToolBar actions
      QList<QAction *> m_activeToolBarActions; //!< List of active ToolBar actions
      QList<QAction *> m_toolPadActions; //!< List of ToolPad actions

  };
}

#endif // Directory_H
