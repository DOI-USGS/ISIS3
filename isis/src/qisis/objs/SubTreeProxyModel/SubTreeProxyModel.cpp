#include "ObservationSolveSettingsProxyModel.h"

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

  SubTreeProxyModel::SubTreeProxyModel(QObject *parent) :
      QIdentityProxyModel(parent) {
  }


  QModelIndex SubTreeProxyModel::mapFromSource(const QModelIndex &sourceIndex) const {
    // return QIdentityProxyModel::mapFromSource(sourceIndex);
    qDebug() << "BEGIN mapFromSource(" << sourceIndex << ")";
    // QModelIndex parent = sourceIndex.parent();
    // int row = sourceIndex.row();
    // if (!sourceIndex.isValid()) {
    //   return m_root;
    // }

    // check if the model index corresponds to the invisible root item in source model
    if (sourceIndex == static_cast<QStandardItemModel *>(sourceModel())->invisibleRootItem()->index()) {
      return createIndex(sourceIndex.row(), 0, sourceIndex.internalId());
    }

    // First check to see if the source index is the proxy root
    if (sourceIndex == m_root) {
      qDebug() << "source exactly matches root already.";
      return createIndex(sourceIndex.row(), 0, sourceIndex.internalId());
    }

    // If the source index is a child of the proxy root, one if its ancestors IS proxy root
    QModelIndex ancestorIndex = sourceIndex.parent();
    QString n = "";
    qDebug() << ancestorIndex;
    while (ancestorIndex.isValid() && ancestorIndex != m_root) {
      ancestorIndex = ancestorIndex.parent();
      n += "\t";
      qDebug() << n << ancestorIndex;
    }
    if (ancestorIndex.isValid()) {
      return createIndex(sourceIndex.row(), 0, sourceIndex.internalId());
      // return QModelIndex(parent.child(row));
    }
    else {
      return QModelIndex();
    }

    // int sourceRow = sourceIndex.row();
    // int rootRow = m_root.row();

    // Siblings of the proxy root are NOT part of the subtree
    /* Item 0        (the source tree root; the proxy root's parent)
     * - Item 0      (a sibling of the proxy root)
     * - Item 1      (the proxy root, m_root)
     *   - Item 0    (a child of the proxy root)
     * - Item 2      (a sibling of the proxy root)
    //  */
    // if (sourceIndex.parent() == m_root.parent()) {
    //   return QModelIndex();
    // }

    // // Ancestors of the proxy root are NOT part of the subtree
    // QModelIndex parentIndex = sourceIndex.parent();
    // while (parentIndex.isValid()) {
    //   parentIndex = parentIndex.parent();
    // }


    // if (sourceRow != rootRow) {
    //   return QModelIndex();
    // }
    // QModelIndex sourceParent = sourceIndex.parent();

    // See if there 
    // Check to see if we are at the root
    // if (!sourceParent.isValid()) {
      
    // }
    // else {

    // }
    // int targetRow = sourceRow;
    // return QIdentityProxyModel::mapFromSource(sourceIndex);
  }


  QModelIndex SubTreeProxyModel::mapToSource(const QModelIndex &proxyIndex) const {
    // return proxyIndex;
    return QIdentityProxyModel::mapToSource(proxyIndex);
  }


  void SubTreeProxyModel::setSourceModel(QAbstractItemModel *newSourceModel) {
    // QVariant data = newSourceModel->data(newSourceModel->index(0,0,QModelIndex()));
    // qDebug() << data;
    // qDebug() << "can convert to project item: " << data.canConvert<ProjectItem *>();
    qDebug() << typeid(*newSourceModel).name();
    
    // QStandardItem *item = static_cast<QStandardItemModel *>(newSourceModel)->invisibleRootItem();
    QPersistentModelIndex persistentIndex(newSourceModel->index(0,0,QModelIndex()));
    // QPersistentModelIndex persistentIndex(item->index());
    // qDebug() << "root item: " << item;
    // qDebug() << "root item index: " << item->index();

    
    if (persistentIndex.isValid()) {
      qDebug() << "persistent index is valid: " << persistentIndex;
      qDebug() << "parent index: " << persistentIndex.parent();
      m_root = persistentIndex;
    }
    else {
      qDebug() << "persistent index NOT valid.";
      m_root = QPersistentModelIndex(QModelIndex());
    }

    // // qDebug() << "can convert to qstandarditem: " << data.canConvert<QStandardItem *>();
    // if (data.canConvert<ProjectItem *>()) {
    //  m_root = data.value<ProjectItem *>();
    // }
    QIdentityProxyModel::setSourceModel(newSourceModel);
  }


  bool SubTreeProxyModel::setRoot(const QStandardItem *item) {
    qDebug() << "setRoot() item->index() " << item->index();
    qDebug() << "\titem->parent()->index() " << item->parent()->index();
    m_root = QPersistentModelIndex(item->index());
    qDebug() << "Perisentent Model Index ROOT: " << m_root << " is valid ? " << (m_root.isValid() ? "yes" : "no");
    qDebug() << "Persistent's PARENT: " << m_root.parent();
    if (m_root.isValid()) {
      return true;
    }
    else {
      return false;
    }
  }
  
  // bool ObservationSolveSettingsProxyModel::filterAcceptsRow(int sourceRow, 
  //                                                           const QModelIndex &sourceParent) const {
  //   QModelIndex mIndex = sourceModel()->index(sourceRow, 0, sourceParent);

  // }


}