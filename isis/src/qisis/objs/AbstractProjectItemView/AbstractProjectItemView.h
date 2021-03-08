#ifndef AbstractProjectItemView_h
#define AbstractProjectItemView_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QMainWindow>

class QAction;
class QDragEnterEvent;
class QWidget;
template <typename T> class QList;

namespace Isis {

  class ProjectItem;
  class ProjectItemModel;

  /**
   * AbstractProjectItemView is a base class for views of a
   * ProjectItemModel in Qt's model-view
   * framework. AbstractProjectItemView is not meant to be
   * instantiated directly. A view usually only shows items that have
   * been added to the view. The views contains an internal
   * ProjectItemProxyModel that represents the items appropriately for
   * the view.
   *
   * When mime data is dropped on a view the view adds the selected
   * items from the source model to the view.
   *
   * Note that AbstractProjectItemView does not inherit from QAbstractItemView.
   *
   * @author 2015-10-21 Jeffrey Covington
   * @internal
   *   @history 2015-10-21 Jeffrey Covington - Original version.
   *   @history 2016-06-27 Ian Humphrey - Minor updates to documentation and coding standards.
   *                           Fixes #4004.
   *   @history 2016-07-28 Tracie Sucharski - Implemented removeItem and removeItems methods.
   *   @history 2016-08-25 Adam Paquette - Minor updates to documentation.
   *                           Fixes #4299.
   *   @history 2018-05-29 Tracie Sucharski & Summer Stapleton - updated to inherit from QMainWindow
   *                           instead of QWidget. This updates all views in the ipce main window
   *                           to be main windows themselves, changing from an mdi interface to an
   *                           sdi interface.
   *   @history 2018-05-30 Tracie Sucharski - Added the WindowFlag to set this as a Widget.
   *   @history 2018-06-15 Kaitlyn Lee - Removed methods returing toolbar and menu actions because
   *                            each individual has its own toolbar. These methods are not needed
   *                            anymore.
   *   @history 2018-06-18 Summer Stapleton - Overloaded moveEvent and resizeEvent and added a
   *                           windowChangeEvent signal to allow project to recognize a new save
   *                           state. Fixes #5114
   *   @history 2018-06-25 Kaitlyn Lee - When multiple views are open, there is a possibility of
   *                           getting ambiguous shortcut errors. To counter this, we need a way to
   *                           focus on one widget. Giving the views focus did not work completely.
   *                           Instead, enabling/disabling actions was the best option. Added
   *                           enableActions(), disableActions(), enterEvent(), and leaveEvent(). On
   *                           default, a view's actions are disabled. To enable the actions, move
   *                           the cursor over the view. When a user moves the cursor outside of the
   *                           view, the actions are disabled.
   *   @history 2018-07-05 Tracie Sucharski - Added SizePolicy and a large sizeHint.  The large
   *                           sizeHint() is because using sizePolicy with a reasonable sizeHint did
   *                           not work to have views fill the available space in the dock area.
   *                           References #5433.
   *   @history 2018-07-12 Kaitlyn Lee - Changed the sizeHint to be calculated based on the deskTop
   *                           size, instead of being hard-coded. The percentages chosen allow for 2
   *                           CubeDnViews to be opened at once, since CubeDnView has an internal
   *                           size policy. References #5433
   *   @history 2018-07-26 Tracie Sucharski - Cleaned up some documentation.
   *   @history 2018-08-10 Tracie Sucharski - Changed addItems method to call the
   *                           ProjectItemProxyModel::addItems rather than this classes addItem.
   *                           This speeds things up considerably loading items into the model.
   *                           References #5296.
   */
  class AbstractProjectItemView : public QMainWindow {

    Q_OBJECT

    public:
      AbstractProjectItemView(QWidget *parent=0);

      virtual QSize sizeHint() const;

      virtual void setModel(ProjectItemModel *model);
      virtual ProjectItemModel *model();

      virtual void dragEnterEvent(QDragEnterEvent *event);
      virtual void dragMoveEvent(QDragMoveEvent *event);
      virtual void dropEvent(QDropEvent *event);

      virtual void moveEvent(QMoveEvent *event);
      virtual void resizeEvent(QResizeEvent *event);

      virtual void enterEvent(QEvent *event);
      virtual void leaveEvent(QEvent *event);
      virtual void enableActions();

      virtual QList<QAction *> contextMenuActions();

      virtual ProjectItem *currentItem();
      virtual QList<ProjectItem *> selectedItems();

      virtual ProjectItemModel *internalModel();
      virtual void setInternalModel(ProjectItemModel *model);

    signals:
      void windowChangeEvent(bool event);

    public slots:
      virtual void addItem(ProjectItem *item);
      virtual void addItems(QList<ProjectItem *> items);

      virtual void removeItem(ProjectItem *item);
      virtual void removeItems(QList<ProjectItem *> items);

      virtual void disableActions();

    private:
      ProjectItemModel *m_internalModel; //!< The internal model used by the view
  };
}

#endif
