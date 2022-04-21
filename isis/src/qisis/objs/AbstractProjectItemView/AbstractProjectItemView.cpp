/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractProjectItemView.h"

#include <QAction>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QList>
#include <QMainWindow>
#include <QRect>
#include <QSizePolicy>
#include <QWidget>
#include <QGuiApplication>
#include <QScreen>

#include "ProjectItem.h"
#include "ProjectItemModel.h"
#include "ProjectItemProxyModel.h"

namespace Isis {

  /**
   * Constructs the AbstractProjectItemView.
   *
   * @param[in] parent (QMainWindow *) The parent widget
   */
  AbstractProjectItemView::AbstractProjectItemView(QWidget *parent) : QMainWindow(parent) {

    setWindowFlags(Qt::Widget);

    QSizePolicy policy = sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Expanding);
    policy.setVerticalPolicy(QSizePolicy::Expanding);
    setSizePolicy(policy);

    m_internalModel = new ProjectItemProxyModel(this);
    setAcceptDrops(true);
  }


  /**
   * Returns the suggested size
   *
   * @return @b QSize The size hint
   */
  QSize AbstractProjectItemView::sizeHint() const {

    //  Size hint is made large as a hack to have the views fill the available dock
    //  space. SizePolicy alone did not work.
    QRect availableSpace = (QGuiApplication::primaryScreen()->availableGeometry());
    return QSize( .89 * availableSpace.width(), .5 * availableSpace.height() );
  }


  /**
   * Sets the model used by the view. If the internal model is a proxy
   * model, it sets the source model.
   *
   * @param[in] model (ProjectItemModel *) The new model
   */
  void AbstractProjectItemView::setModel(ProjectItemModel *model) {
    if (ProjectItemProxyModel *proxyModel =
        qobject_cast<ProjectItemProxyModel *>( internalModel() ) ) {
      proxyModel->setSourceModel(model);
    }
  }


  /**
   * Returns the model used by the view. If the internal model is a
   * proxy model, it returns the source model.
   *
   * @return @b ProjectItemModel * The model.
   */
  ProjectItemModel *AbstractProjectItemView::model() {
    if (ProjectItemProxyModel *proxyModel =
        qobject_cast<ProjectItemProxyModel *>( internalModel() ) ) {
      return proxyModel->sourceModel();
    }
    return internalModel();
  }


  /**
   * Sets the internal model of the view.
   *
   * @param[in] model (ProjectItemModel *) The new internal model
   */
  void AbstractProjectItemView::setInternalModel(ProjectItemModel *model) {
    m_internalModel = model;
  }


  /**
   * Returns the internal model of the view. By default it is a proxy
   * model.
   *
   * @return @b ProjectItemModel * The internal model
   */
  ProjectItemModel *AbstractProjectItemView::internalModel() {
    return m_internalModel;
  }


  /**
   * Accepts the drag enter event if the internal model can accept the
   * mime data.
   *
   * @param[in] event (QDragEnterEvent *) The drag event
   */
  void AbstractProjectItemView::dragEnterEvent(QDragEnterEvent *event) {
    if (internalModel()->canDropMimeData( event->mimeData(),
                                          event->dropAction(),
                                          0, 0, QModelIndex() ) ) {
      event->acceptProposedAction();
    }
  }


  /**
   * Accepts the drag event if the internal model can accept the mime
   * data.
   *
   * @param[in] event (QDragMoveEvent *) The drag event
   */
  void AbstractProjectItemView::dragMoveEvent(QDragMoveEvent *event) {
    if (internalModel()->canDropMimeData( event->mimeData(),
                                          event->dropAction(),
                                          0, 0, QModelIndex() ) ) {
      event->acceptProposedAction();
    }
  }


  /**
   * Drops the data into the internal model if it can accept the data.
   *
   * @param[in] event (QDropEvent *) The drop event
   */
  void AbstractProjectItemView::dropEvent(QDropEvent *event) {
    if (internalModel()->canDropMimeData( event->mimeData(),
                                          event->dropAction(),
                                          0, 0, QModelIndex() ) ) {
      internalModel()->dropMimeData( event->mimeData(), event->dropAction(),
                                     0, 0, QModelIndex() );
      event->acceptProposedAction();
    }
  }


  void AbstractProjectItemView::moveEvent(QMoveEvent *event) {
    QMainWindow::moveEvent(event);

    emit windowChangeEvent(false);
  }


  void AbstractProjectItemView::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    emit windowChangeEvent(false);
  }


  /**
   * Enables actions when cursor enters the view
   *
   * @param event The enter event
   */
  void AbstractProjectItemView::enterEvent(QEvent *event) {
    enableActions();
  }


  /**
   * Disables actions when cursor leaves the view.
   *
   * @param event The leave event
   */
  void AbstractProjectItemView::leaveEvent(QEvent *event) {
    disableActions();
  }


  /**
   * Disables toolbars and toolpad actions
   */
  void AbstractProjectItemView::disableActions() {
    foreach (QAction *action, actions()) {
      action->setDisabled(true);
    }
  }


  /**
   * Enables toolbars and toolpad actions
   */
  void AbstractProjectItemView::enableActions() {
    foreach (QAction *action, actions()) {
      action->setEnabled(true);
    }
  }


  /**
   * Returns a list of actions appropriate for a context menu.
   *
   * @return @b QList<QAction *> The actions
   */
  QList<QAction *> AbstractProjectItemView::contextMenuActions() {
    return QList<QAction *>();
  }


  /**
   * Returns the current item of the model.
   *
   * @return @b ProjectItem * The item
   */
  ProjectItem *AbstractProjectItemView::currentItem() {
    return model()->currentItem();
  }


  /**
   * Return the selected items of the model.
   *
   * @return @b QList<ProjectItem *> The items
   */
  QList<ProjectItem *> AbstractProjectItemView::selectedItems() {
    return model()->selectedItems();
  }


  /**
   * Adds an item to the view. The item must be part of the view's
   * model. This method can be overridden in a subclass to filter out
   * unneeded items.
   *
   * @param[in] item (ProjectItem *) The item to add.
   */
  void AbstractProjectItemView::addItem(ProjectItem *item) {
    if (ProjectItemProxyModel *proxyModel = qobject_cast<ProjectItemProxyModel *>( internalModel() )) {
      proxyModel->addItem(item);
    }
  }


  /**
   * Adds several items to the view. The items must be a part of the
   * view's model. This method can be overridden in a subclass to filter out
   * unneeded items.
   *
   * @param[in] items (QList<ProjectItem *>) The items to add.
   */
  void AbstractProjectItemView::addItems(QList<ProjectItem *> items) {
    if (ProjectItemProxyModel *proxyModel = qobject_cast<ProjectItemProxyModel *>( internalModel() )) {
      proxyModel->addItems(items);
    }
  }


  /**
   * Removes an item to the view. The item must be part of the view's
   * model. This method can be overriden in a subclass to filter out
   * unneeded items.
   *
   * @param[in] item (ProjectItem *) The item to remove.
   */
  void AbstractProjectItemView::removeItem(ProjectItem *item) {
    if (ProjectItemProxyModel *proxyModel =
            qobject_cast<ProjectItemProxyModel *>( internalModel() ) ) {
      proxyModel->removeItem(item);
    }
  }


  /**
   * Removes several items from the view. The items must be a part of the
   * view's model.
   *
   * @param[in] items (QList<ProjectItem *>) The items to remove.
   */
  void AbstractProjectItemView::removeItems(QList<ProjectItem *> items) {
    foreach (ProjectItem *item, items) {
      removeItem(item);
    }
  }
}
