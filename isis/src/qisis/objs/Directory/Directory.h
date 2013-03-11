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

#include "ImageList.h"
#include "MosaicSceneWidget.h"
#include "WorkOrder.h"

class QAction;
class QDockWidget;
class QMenuBar;
class QProgressBar;
class QSplitter;
class QTabWidget;

namespace Isis {
  class CnetEditorWidget;
  class ControlNet;
  class HistoryTreeWidget;
  class ImageFileListWidget;
  class MosaicSceneWidget;
  class Project;
  class ProjectTreeWidget;
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
   */
  class Directory : public QObject {
    Q_OBJECT
    public:
      explicit Directory(QObject *parent = 0);
      ~Directory();

      void populateMainMenu(QMenuBar *);
      void setHistoryContainer(QDockWidget *historyContainer);
      void setWarningContainer(QDockWidget *warningContainer);

      CnetEditorWidget *addCnetEditorView(Control *network);
      Workspace *addCubeDnView();
      MosaicSceneWidget *addFootprint2DView();
      ImageFileListWidget *addImageFileListView();
      QWidget *projectTreeWidget();

      Project *project() const;

      QList<CnetEditorWidget *> cnetEditorViews();
      QList<Workspace *> cubeDnViews();
      QList<MosaicSceneWidget *> footprint2DViews();
      QList<ImageFileListWidget *> imageFileListViews();
      QList<QProgressBar *> progressBars();

      template <typename DataType>
      QList<QAction *> supportedActions(DataType data) {
        QList<QAction *> results;

        QList< QPair< QString, QList<QAction *> > > actionPairings;

        foreach (MosaicSceneWidget *footprint2DView, m_footprint2DViewWidgets) {
          actionPairings.append(
              qMakePair(footprint2DView->windowTitle(), footprint2DView->supportedActions(data)));
        }

        results.append(restructureActions(actionPairings));

        if (!results.isEmpty()) {
          results.append(NULL);
        }

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

      template <typename Data>
      void showWarning(QString text, Data data) {
//        m_warningTreeWidget->showWarning(text, data);
      }

      QWidget *warningWidget();

      QAction *redoAction();
      QAction *undoAction();

      void load(XmlStackedHandlerReader *xmlReader);
      void save(QXmlStreamWriter &stream, FileName newProjectRoot) const;

    signals:
      void newWidgetAvailable(
          QWidget *newWidget, Qt::DockWidgetArea area, Qt::Orientation orientation);

    public slots:
      void cleanupCnetEditorViewWidgets();
      void cleanupCubeDnViewWidgets();
      void cleanupFileListWidgets();
      void cleanupFootprint2DViewWidgets();
      void imagesAddedToProject(ImageList *images);

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

          Directory *m_directory;
      };

    private:
      Directory(const Directory &other);
      Directory &operator=(const Directory &rhs);

      /**
       * Create a work order, append it to m_workOrders, and return it.
       *
       * Example:
       *   createWorkOrder<ImageFileListViewWorkOrder>();
       *   This will create a new ImageFileListViewWorkOrder and append it to m_workOrders.
       *
       */
      template <typename WorkOrderType>
      WorkOrderType *createWorkOrder() {
        WorkOrderType *newWorkOrder = new WorkOrderType(m_project);
        m_workOrders.append(newWorkOrder);
        return newWorkOrder;
      }

      static QList<QAction *> restructureActions(QList< QPair< QString, QList<QAction *> > >);
      static bool actionTextLessThan(QAction *lhs, QAction *rhs);

      QPointer<ProjectTreeWidget> m_projectTreeWidget;
      QPointer<HistoryTreeWidget> m_historyTreeWidget;
      QPointer<Project> m_project;
      QPointer<WarningTreeWidget> m_warningTreeWidget;

      QList< QPointer<CnetEditorWidget> > m_cnetEditorViewWidgets;
      QList< QPointer<Workspace> > m_cubeDnViewWidgets;
      QList< QPointer<ImageFileListWidget> > m_fileListWidgets;
      QList< QPointer<MosaicSceneWidget> > m_footprint2DViewWidgets;

      QList< QPointer<WorkOrder> > m_workOrders;

      // We only need to store the work orders that go into menus uniquely... all work orders
      //   (including these) should be stored in m_workOrders
      QPointer<WorkOrder> m_exportControlNetWorkOrder;
      QPointer<WorkOrder> m_exportImagesWorkOrder;
      QPointer<WorkOrder> m_importControlNetWorkOrder;
      QPointer<WorkOrder> m_importImagesWorkOrder;
      QPointer<WorkOrder> m_openProjectWorkOrder;
      QPointer<WorkOrder> m_saveProjectWorkOrder;
      QPointer<WorkOrder> m_saveProjectAsWorkOrder;

      QPointer<WorkOrder> m_renameProjectWorkOrder;
  };
}

#endif // Directory_H
