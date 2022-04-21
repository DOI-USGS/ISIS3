/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ProjectItemTreeView.h"

#include <QAbstractItemView>
#include <QEvent>
#include <QObject>
#include <QRect>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>
#include <QGuiApplication>
#include <QScreen>

#include "ProjectItem.h"
#include "ProjectItemModel.h"

namespace Isis {
  /**
   * Constructs a ProjectItemTreeView.
   *
   * @param[in] parent (QWidget *) The parent widget.
   */
  ProjectItemTreeView::ProjectItemTreeView(QWidget *parent) : AbstractProjectItemView(parent) {

    m_treeView = new QTreeView(this);
    m_treeView->installEventFilter(this);

    setInternalModel( internalModel() );
    // 2017-04-12 TSucharski Turn off for now, since not accepting drops, not point in allowing
    // drags
//  m_treeView->setDragEnabled(true);
    m_treeView->setDragEnabled(false);
    m_treeView->setAcceptDrops(false);
    m_treeView->setHeaderHidden(true);

    //  Simply doing this creates scrollbar when the dock widget within the main window is made too
    //  small- looks like QMainWindow and/or QDockWidget handles the scrollbars for us.
    setCentralWidget(m_treeView);

    //This works so that it cannot be shrunk in width, only grown
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    //  Currently set all items on view to un-editable
    //m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

//  setAcceptDrops(true);
  }


  /**
   * Returns the suggested size
   *
   * @return @b QSize The size hint
   */
  QSize ProjectItemTreeView::sizeHint() const {
    QRect availableSpace = QGuiApplication::primaryScreen()->availableGeometry();
    return QSize(.15 * availableSpace.width(), .5 * availableSpace.height());
  }


  /**
   * Default destructor.
   */
  ProjectItemTreeView::~ProjectItemTreeView() {}


  /**
   * Returns the tree view.
   *
   * @return @b QTreeView* The tree view.
   */
  QTreeView *ProjectItemTreeView::treeView() {
    return m_treeView;
  }


  /**
   * Sets the model so that the internal proxy model exactly matches the
   * source model.
   *
   * @param[in] model (ProjectItemModel *) The source model.
   */
  void ProjectItemTreeView::setInternalModel(ProjectItemModel *model) {
    disconnect(internalModel(), 0, this, 0);

    AbstractProjectItemView::setInternalModel(model);

    m_treeView->reset();
    m_treeView->setModel(model);
    m_treeView->setSelectionModel( model->selectionModel() );
    m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect( model, SIGNAL( itemAdded(ProjectItem *) ),
             this, SLOT( onItemAdded(ProjectItem *) ) );

  }


  /**
   * Expands the parent project item in the tree view to show the added item.
   *
   * @param[in] item (ProjectItem *) The added project item.
   */
  void ProjectItemTreeView::onItemAdded(ProjectItem *item) {
    ProjectItem *parent = item->parent();
    if (!parent) {
      return;
    }
    if ( !parent->isImageList() && !parent->isControlList() ) {
      m_treeView->expand( parent->index() );
    }
  }


  /**
   * Filters out drag and drop events so that they are handled by the
   * ProjectItemTreeView.
   *
   * @param[in] watched (QObject *) The object being filtered.
   * @param[in] event (QEvent *) The event passed to the object.
   *
   * @return @b bool True if the event is filtered by the project item tree view
   */
  bool ProjectItemTreeView::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::DragEnter) {
      return true;
    }
    else if (event->type() == QEvent::Drop) {
      return true;
    }

    return AbstractProjectItemView::eventFilter(watched, event);
  }

}
