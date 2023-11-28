#ifndef CubeDnView_h
#define CubeDnView_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QList>
#include <QMap>
#include <QWidgetAction>

#include "AbstractProjectItemView.h"
#include "FileName.h"
#include "XmlStackedHandler.h"

class QAction;
class QMenu;
class QModelIndex;
class QToolBar;
class QXmlStreamWriter;
class QWidget;

namespace Isis {

  class ControlPoint;
  class Cube;
  class Directory;
  class Image;
  class ImageList;
  class MdiCubeViewport;
  class Project;
  class ToolPad;
  class Workspace;
  class ProjectItemViewMenu;

  /**
   * View that displays cubes in a QView-like way.
   *
   * @author 2016-01-13 Jeffrey Covington
   *
   * @internal
   *   @history 2016-01-13 Jeffrey Covington - Original version.
   *   @history 2016-06-27 Ian Humphrey - Minor updates to documentation and coding standards.
   *                           Fixes #4004.
   *   @history 2016-08-25 Adam Paquette - Updated documentation. Fixes #4299.
   *   @history 2016-09-14 Tracie Sucharski - Replaced QnetTool with IpceTool.Added signals for
   *                           mouse clicks for modifying, deleting and creating control points.
   *                           These are passed on to Directory slots.
   *   @history 2016-10-18 Tracie Sucharski - Added the status bar back in in order to display cube
   *                           positional information (sample, line, latitude, longitude).
   *   @history 2016-10-18 Tracie Sucharski - Add method to return whether the viewport contains a
   *                           Shape.
   *   @history 2016-11-10 Tracie Sucharski - Added functionality to save/restore CubeDnViews when
   *                           opening projects.
   *   @history 2017-01-02 Tracie Sucharski - Moved IpceTool to first tool on tool bar, change
   *                           icon to match the Footprint2DView.
   *   @history 2017-05-18 Tracie Sucharski - Added serialNumber to the modifyControlPoint signal.
   *   @history 2017-07-27 Tyler Wilson - Added a slot function (enableIPCETool) which is called
   *                           when a control network (or list of control networks) is added to
   *                           the active project.  Fixes #4994.
   *   @history 2017-08-02 Tracie Sucharski - Added redrawMeasures signal which calls a slot in
   *                           ipce tool to redraw control measures on cube viewports. Fixes #5007,
   *                           #5008.
   *   @history 2017-08-03 Cole Neubauer - Changed all references from IpceTool to ControlNetTool
   *                           Fixes #5090.
   *   @history 2018-05-30 Tracie Sucharski - Refactored for new docked interface.  Views are now
   *                           responsible for creating their menus and toolbars.
   *                           AbstractProjectItemView now inherits from QMainWindow, so the
   *                           Workspace of this view is the centralWidget. This needs further work
   *                           to cleanup and fit in with the new docked view interface.git
   *   @history 2018-06-12 Kaitlyn Lee - Removed help menu and the "What's This?" action because the
   *                           ipce help menu has this action.
   *   @history 2018-06-13 Kaitlyn Lee - Since views now inherit from QMainWindow, each individual
   *                           view has its own toolbar, so having getters that return toolbar
   *                           actions to fill the toolbar of the IpceMainWindow are unnecessary.
   *                           Removed methods that returned menu and toolbar actions.
   *                           Removed connections that connected the project and CubeDnView and
   *                           called enableControlNetTool() because Directory now does this.
   *   @history 2018-06-25 Kaitlyn Lee - When multiple views are open, there is a possibility of
   *                           getting ambiguous shortcut errors. To counter this, we enable/disable
   *                           actions. Overrode leaveEvent() to handle open menus causing a leave
   *                           event. Overrode enable/disableActions() because we need to disable
   *                           the active toolbar's widgets. On default, a view's actions are
   *                           disabled. To enable the actions, move the cursor over the view. When
   *                           a user moves the cursor outside of a view, the actions are disabled.
   *   @history 2018-07-05 Tracie Sucharski - Moved sizeHint and sizePolicy to
   *                           AbstractProjectItemView. References #5433.
   *   @history 2018-07-09 Tracie Sucharski - Serialize the objectName for this view so that the
   *                           view can be re-created with the same objectName for restoring the
   *                           project state. Qt's save/restoreState use the objectName.
   */
  class CubeDnView : public AbstractProjectItemView {

    Q_OBJECT

    public:
      CubeDnView(Directory *directory, QWidget *parent=0);
      ~CubeDnView();

      bool viewportContainsShape(MdiCubeViewport *viewport);

      void save(QXmlStreamWriter &stream, Project *project, FileName newProjectRoot) const;

    signals:
      void modifyControlPoint(ControlPoint *controlPoint, QString serialNumber);
      void deleteControlPoint(ControlPoint *controlPoint);
      void createControlPoint(double latitude, double longitude, Cube *cube,
                              bool isGroundSource = false);

      void controlPointAdded(QString newPointId);
      void redrawMeasures();

    public slots:
      void addItem(ProjectItem *item);
      void enableControlNetTool(bool value);

    private slots:
      void createActions(Directory *directory);

      void onCurrentChanged(const QModelIndex &current);
      void onCubeViewportActivated(MdiCubeViewport *);
      void onItemAdded(ProjectItem *item);
      void onCubeViewportAdded(MdiCubeViewport *viewport);
      void onCubeViewportDeleted(QObject *obj);
      void disableActions();

    private:
      Cube *workspaceActiveCube();
      void setWorkspaceActiveCube(Image *image);
      void leaveEvent(QEvent *event);
      void enableActions();

    private:
      /**
       * @author 2012-09-?? Steven Lambright
       *
       * @internal
       *   @history 2016-11-07 Tracie Sucharski - Implemented for CubeDnView
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(CubeDnView *cubeDnView, Project *project);
          ~XmlHandler();

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);
          virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                  const QString &qName);

        private:
          Q_DISABLE_COPY(XmlHandler);

          Project *m_project;       //!< The current project
          CubeDnView *m_cubeDnView; //!< The view we are working with
      };

    private:
      QMap<Cube *, ProjectItem *> m_cubeItemMap; //!< Maps cubes to their items
      Workspace *m_workspace; //!< The workspace
      Directory *m_directory; //!< The directory

      ProjectItemViewMenu *m_viewMenu; //!< View menu for storing actions
      ProjectItemViewMenu *m_optionsMenu; //!< Options menu for storing actions
      ProjectItemViewMenu *m_windowMenu; //!< Window menu for storing actions

      QAction *m_separatorAction; //!< A separator action that is reused

      QToolBar *m_permToolBar; //!< A tool bar for storing actions
      QToolBar *m_activeToolBar; //!< A tool bar for storing actions
      ToolPad *m_toolPad; //!< A tool bar for storing actions
      QList<QWidget *> m_childWidgets;  //!< Child widgets of the active toolbar
  };
}

#endif
