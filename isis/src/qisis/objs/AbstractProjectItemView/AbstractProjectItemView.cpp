/**
 * @file
 * $Date$
 * $Revision$
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
#include "AbstractProjectItemView.h"

#include <QtGui>

#include "ProjectItem.h"
#include "ProjectItemModel.h"
#include "ProjectItemProxyModel.h"

namespace Isis {

  /**
   * Constructs the AbstractProjectItemView
   *
   * @param[in] parent (QWidget *)
   */
  AbstractProjectItemView::AbstractProjectItemView(QWidget *parent) : QWidget(parent) {
    m_proxyModel = new ProjectItemProxyModel(this);
    setAcceptDrops(true);
  }


  /**
   * 
   *
   * @param[in] model (ProjectItemModel *)
   */
  void AbstractProjectItemView::setModel(ProjectItemModel *model) {
    proxyModel()->setSourceModel(model);
  }


  /**
   * @param[in] event (QDragEnterEvent *)
   */
  void AbstractProjectItemView::dragEnterEvent(QDragEnterEvent *event) {
    event->acceptProposedAction();
  }


  /**
   * @param[in] event (QDragMoveEvent *)
   */
  void AbstractProjectItemView::dragMoveEvent(QDragMoveEvent *event) {
    event->acceptProposedAction();
  }

  /**
   * @param[in] event (QDropEvent *)
   */
  void AbstractProjectItemView::dropEvent(QDropEvent *event) {
    addItems( proxyModel()->sourceModel()->selectedItems() );
    event->acceptProposedAction();
  }


  /**
   * @return (QList<QAction *>)
   */
  QList<QAction *> AbstractProjectItemView::menuActions() {
    return toolBarActions();
  }


  /**
   * @return (QList<QAction *>)
   */
  QList<QAction *> AbstractProjectItemView::toolBarActions() {
    return QList<QAction *>();
  }


  /**
   * @return (QList<QAction *>)
   */
  QList<QAction *> AbstractProjectItemView::contextMenuActions() {
    return menuActions();
  }


  /**
   * @return (ProjectItem *)
   */
  ProjectItem *AbstractProjectItemView::currentItem() {
    return proxyModel()->sourceModel()->currentItem();
  }


  /**
   * @return (QList<ProjectItem *>)
   */
  QList<ProjectItem *> AbstractProjectItemView::selectedItems() {
    return proxyModel()->selectedItems();
  }

  
  /**
   * @param[in] item (ProjectItem *)
   */
  void AbstractProjectItemView::addItem(ProjectItem *item) {
    proxyModel()->addItem(item);
  }


  /**
   * @param[in] items (QList<ProjectItem *>)
   */
  void AbstractProjectItemView::addItems(QList<ProjectItem *> items) {
    foreach (ProjectItem *item, items) {
      addItem(item);
    }
  }
  

  /**
   * @return (ProjectItemProxyModel *)
   */
  ProjectItemProxyModel *AbstractProjectItemView::proxyModel() {
    return m_proxyModel;
  }


  /**
   * @param[in] proxyModel (ProjectItemProxyModel *)
   */
  void AbstractProjectItemView::setProxyModel(ProjectItemProxyModel *proxyModel) {
    m_proxyModel = proxyModel;
  }


}
