#ifndef Footprint2DView_h
#define Footprint2DView_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QList>
#include <QMap>
#include <QSize>

#include "AbstractProjectItemView.h"
#include "FileName.h"
#include "ImageList.h"

class QAction;
class QEvent;
class QMainWindow;
class QToolBar;
class QWidgetAction;
class QXmlStreamWriter;

namespace Isis {

  class ControlPoint;
  class Directory;
  class Image;
  class ImageFileListWidget;
  class MosaicSceneWidget;
  class Project;
  class ToolPad;

  /**
   * View for displaying footprints of images in a QMos like way.
   *
   * @author 2016-01-13 Jeffrey Covington
   *
   * @internal
   *   @history 2016-01-13 Jeffrey Covington - Original version.
   *   @history 2016-06-27 Ian Humphrey - Minor updates to documentation, checked coding standards.
   *                           Fixes #4004.
   *   @history 2016-08-25 Adam Paquette - Updated documentation. Fixes #4299.
   *   @history 2016-09-14 Tracie Sucharski - Added signals for mouse clicks for modifying, deleting
   *                           and creating control points.  These are passed on to Directory slots.
   *   @history 2016-10-20 Tracie Sucharski -  Added back the capability for choosing either a new
   *                           view or using an existing view.
   *   @history 2017-02-06 Tracie Sucharski - Added status bar for the track tool.  Fixes #4475.
   *   @history 2017-07-18 Cole Neubauer - Moved creation of the ImageFileListWidget into
   *                           Footprint2DView to more mirror the Qmos window.  Fixes #4996.
   *   @history 2017-07-27 Makayla Shepherd - Fixed a segfault that occurred when closing a cube
   *                           footprint. Fixes #5050.
   *   @history 2017-08-02 Tracie Sucharski - Fixed connections between views for control point
   *                           editing.  Fixes #5007, #5008.
   *   @history 2018-05-14 Tracie Sucharski - Serialize Footprint2DView rather than
   *                           MosaicSceneWidget. This will allow all parts of Footprint2DView to be
   *                           saved/restored including the ImageFileListWidget. Fixes #5422.
   *   @history 2018-05-30 Summer Stapleton - updated the view to remove QMainWindow constructor,
   *                           include a central widget and to remove layout capacity. This change
   *                           was made to adjust to parent class now inheriting from QMainWindow
   *                           instead of QWidget. References #5433.
   *   @history 2018-06-08 Tracie Sucharski - Remove deletion of m_window from destructor. This
   *                           member variable no longer exists.
   *   @history 2018-06-13 Kaitlyn Lee - Since views now inherit from QMainWindow, each individual
   *                           view has its own toolbar, so having getters that return toolbar
   *                           actions to fill the toolbar of the IpceMainWindow are unnecessary.
   *                           Removed methods that returned menu and toolbar actions.
   *                           Added enableControlNetTool(bool) so when an active control net is set,
   *                           the tool becomes enabled.
   *  @history 2018-06-25 Kaitlyn Lee - When multiple views are open, there is a possibility of
   *                           getting ambiguous shortcut errors. To counter this, we enable/disable
   *                           actions. On default, actions are disabled until a user moves the
   *                           cursor over the view. When a user moves the cursor outside of the
   *                           view, the actions are disabled.
   *   @history 2018-07-09 Tracie Sucharski - Serialize the objectName for this view so that the
   *                           view can be re-created with the same objectName for restoring the
   *                           project state. Qt's save/restoreState use the objectName. Remove
   *                           sizeHint method which is now taken care of in the parent class,
   *                           AbstractProjectItemView.
   *   @history 2018-07-12 Tracie Sucharski - Renamed m_controlNetTool to m_controlNetToolAction
   *                           to be clear it is not a pointer to the tool.  Add a call to
   *                           the MosaicControlNetTool::loadNetwork in enableControlNetTool.
   *   @history 2018-07-31 Tracie Sucharski - Add accessor method for ImageFileListWidget.
   *   @history 2018-08-10 Tracie Sucharski - Added new slot connected from ProjectItemProxyModel's
   *                           itemsAdded signal which is emitted after all selected items have
   *                           been added to the proxy model.  The images are added to a new private
   *                           member as each item is added to the model through the slot,
   *                           onItemAdded. This allows the FootprintView to put all selected items
   *                           into the scene widget at once rather than individually which speeds
   *                           the display of footprints. Fixes #5296.
   *   @history 2018-10-04 Tracie Sucharski - If adding Shape to footprint view, call Image new
   *                           Image constructor which takes a footprint and id, so that the
   *                           MosaicSceneWidget can properly serialize Shapes that have been added.
   *                           Fixes #5495.
   *
   */
  class Footprint2DView : public AbstractProjectItemView {

    Q_OBJECT

    public:
      Footprint2DView(Directory *directory, QWidget *parent=0);
      ~Footprint2DView();

      MosaicSceneWidget *mosaicSceneWidget();
      ImageFileListWidget *fileListWidget();

      void save(QXmlStreamWriter &stream, Project *project, FileName newProjectRoot) const;

    signals:
      void modifyControlPoint(ControlPoint *controlPoint);
      void deleteControlPoint(ControlPoint *controlPoint);
      void createControlPoint(double latitude, double longitude);

      void redrawMeasures();
      void controlPointAdded(QString newPointId);

    public slots:
      void enableControlNetTool(bool value);

    protected:
      bool eventFilter(QObject *watched, QEvent *event);

    private slots:
      void onItemAdded(ProjectItem *item);
      void onItemsAdded();
      void onItemRemoved(ProjectItem *item);
      void onQueueSelectionChanged();
      void onMosItemRemoved(Image *image);

    private:
      void enableActions();

    private:
      MosaicSceneWidget *m_sceneWidget; //!< The scene widget
      ImageFileListWidget *m_fileListWidget; //!< The file list widget
      QMainWindow *m_window; //!< Main window
      ImageList m_images;
      QMap<Image *, ProjectItem *> m_imageItemMap; //!< Maps images to their items
      Directory *m_directory; //!< The directory

      QToolBar *m_permToolBar; //!< The permanent tool bar
      QToolBar *m_activeToolBar; //!< The active tool bar
      ToolPad *m_toolPad; //!< The tool pad
  };
}

#endif
