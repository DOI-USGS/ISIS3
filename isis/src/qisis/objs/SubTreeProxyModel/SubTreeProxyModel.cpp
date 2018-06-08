#include "SubTreeProxyModel.h"

#include <QAbstractItemModel>
#include <QIdentityProxyModel>
#include <QModelIndex>
#include <QObject>
#include <QPersistentModelIndex>
#include <QSortFilterProxyModel>
#include <QStandardItem>
#include <QVariant>

#include "ProjectItem.h"

namespace Isis {


  /**
   * @brief Constructs a SubTreeProxyModel.
   *
   * Constructs a SubTreeProxyModel that can be used to operate on a sub-section of the source
   * model. By default, the sub tree will be identical to the source model. The setRoot() method
   * can be used to specify where the subtree starts. An example of usage can be found in
   * JigsawDialog::createObservationSolveSettingsTreeView().
   *
   * @param QObject *parent The parent of this SubTreeProxyModel.
   *
   * @see JigsawDialog::createObservationSolveSettingsTreeView()
   * @see SubTreeProxyModel::setRoot()
   */
  SubTreeProxyModel::SubTreeProxyModel(QObject *parent) : QIdentityProxyModel(parent) {
  }


  // Returns the model index in the proxy model corresponding to the sourceIndex from the
  // source model
  QModelIndex SubTreeProxyModel::mapFromSource(const QModelIndex &sourceIndex) const {
    // check if the model index corresponds to the invisible root item in source model
    if (sourceIndex == 
            static_cast<QStandardItemModel *>(sourceModel())->invisibleRootItem()->index()) {
      qDebug() << "creating index for invisible root item "
          << static_cast<QStandardItemModel *>(sourceModel())->invisibleRootItem()->index();
      return createIndex(sourceIndex.row(), 0, sourceIndex.internalId());
    }

    // First check to see if the source index is the proxy root
    if (sourceIndex == m_root) {
      return createIndex(sourceIndex.row(), 0, sourceIndex.internalId());
    }

    // If the source index is a child of the proxy root, one if its ancestors IS proxy root
    QModelIndex ancestorIndex = sourceIndex.parent();
    while (ancestorIndex.isValid() && ancestorIndex != m_root) {
      ancestorIndex = ancestorIndex.parent();
    }
    if (ancestorIndex.isValid()) {
      return createIndex(sourceIndex.row(), 0, sourceIndex.internalId());
    }
    else {
      return QModelIndex();
    }
  }


  //Returns the model index in the source that corresponds to the proxy index
  //in the proxy model
  QModelIndex SubTreeProxyModel::mapToSource(const QModelIndex &proxyIndex) const {
    return QIdentityProxyModel::mapToSource(proxyIndex);
  }


  void SubTreeProxyModel::setSourceModel(QAbstractItemModel *newSourceModel) {
    QPersistentModelIndex persistentIndex(newSourceModel->index(0,0,QModelIndex()));

    if (persistentIndex.isValid()) {
      m_root = persistentIndex;
    }
    else {
      m_root = QPersistentModelIndex(QModelIndex());
    }

    QIdentityProxyModel::setSourceModel(newSourceModel);
  }


  bool SubTreeProxyModel::setRoot(const QStandardItem *item) {
    QAbstractItemModel::removeRows(1,2,item->index());

    if (m_root.isValid()) {
      return true;
    }
    else {
      return false;
    }
  }


}
