#ifndef ProjectItemProxyModel_h
#define ProjectItemProxyModel_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QList>
#include <QMap>

#include "ProjectItem.h"
#include "ProjectItemModel.h"

class QItemSelection;
template <typename T> class QList;
class QMimeData;
class QModelIndex;
class QStandardItem;

namespace Isis {
  /**
   * Allows access to items in a ProjectItemModel through a proxy
   * model. A proxy model can have a different structure than the
   * source model. An item in the proxy model usually corresponds to
   * an item in the source model. The proxy model will update item
   * selections and the current item between the source model and the
   * proxy model.
   *
   * In the default implementation the only items in the proxy model
   * are item that are added with the addItem() method. The items that
   * are added are organized in the same tree structure that they are
   * in the source model. Subclasses of ProjectItemProxyModel can
   * organize items in a different way by overriding the addItem()
   * method.
   *
   * The proxy model ensures that its item selection corresponds to
   * the item selection in the source model. When the selection in the
   * proxy model changes the selection in the source model is changed
   * to the items that correspond to the items in the proxy
   * selection. Similarly when the selection in the source model is
   * changed the selection in the proxy model is changed to the items
   * in the proxy model that correpond to the items in the selection.
   *
   * @code
   * ProjectItemModel *model = new ProjectItemModel(this);
   * model->addProject(project);
   * ProjectItemProxyModel *proxyModel = new ProjectItemProxyModel(this);
   * proxyModel->setSourceModel(model);
   * for (int i=0; i < model.rowCount(); i++) {
   *   proxyModel->addItem( model->item(i) );
   * }
   * @endcode
   *
   * @author 2015-10-21 Jeffrey Covington
   *
   * @internal
   *   @history 2015-10-21 Jeffrey Covington - Original version.
   *   @history 2016-01-13 Jeffrey Covington - Added canDropMimeData() and dropMimeData() methods.
   *   @history 2016-06-27 Ian Humphrey - Added documentation to the canDropMimeData() and
   *                           dropMimeData() methods. Checked coding standards. Fixes #4006.
   *   @history 2016-08-11 Tracie Sucharski - Added itemRemoved signal.
   *   @history 2016-08-25 Adam Paquette - Updated documentation. Fixes #4299.
   *   @history 2018-08-10 Tracie Sucharski - Added itemsAdded signal to indicate that all items
   *                           in a list have been added to the model. References #5296.
   */
  class ProjectItemProxyModel : public ProjectItemModel {

  Q_OBJECT

  public:
    ProjectItemProxyModel(QObject *parent = 0);

    QModelIndex mapIndexFromSource(const QModelIndex &sourceIndex);
    QModelIndex mapIndexToSource(const QModelIndex &proxyIndex);

    QItemSelection mapSelectionFromSource(const QItemSelection &sourceSelection);
    QItemSelection mapSelectionToSource(const QItemSelection &proxySelection);

    ProjectItem *mapItemFromSource(ProjectItem *sourceItem);
    ProjectItem *mapItemToSource(ProjectItem *proxyItem);

    void removeItem(ProjectItem *item);

    void setSourceModel(ProjectItemModel *sourceModel);
    ProjectItemModel *sourceModel();

    virtual bool canDropMimeData(const QMimeData *data, Qt::DropAction action,
                                 int row, int column, const QModelIndex &parent) const;
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                              int row, int column, const QModelIndex &parent);

  signals:
    // This signal was added to speed up the Footprint2DView. Previously images were added one at a
    // time which was extremely slow. This not the ideal handling, but without re-writing
    // ProjectItemModel  and ProjectItemProxyModel to add insertRows method so that beginInsertRows
    // and endInsertRows are called which would automatically emit the signal rowsInserted after
    // alls items are inserted into the model rather than calling after each row that is inserted.
    void itemsAdded();
    void itemRemoved(ProjectItem *);

  public slots:
    ProjectItem *addItem(ProjectItem *sourceItem);
    void addItems(QList<ProjectItem *> sourceItems);

  protected slots:
    void updateItem(ProjectItem *sourceItem);
    void updateProxyCurrent();
    void updateSourceCurrent();
    void updateProxySelection();
    void updateSourceSelection();

  protected:
    ProjectItem *addChild(ProjectItem *sourceItem, ProjectItem *parentItem);

  private slots:
    void onItemChanged(QStandardItem *item);

  private:
    ProjectItemModel *m_sourceModel; //!< The source model.
    //! Map of items from the source model to the proxy model.
    QMap<ProjectItem *, ProjectItem *> m_sourceProxyMap;
  };
}
#endif
