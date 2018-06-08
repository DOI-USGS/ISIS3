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
    // return QIdentityProxyModel::mapFromSource(sourceIndex);
    //qDebug() << "BEGIN mapFromSource(" << sourceIndex << ")";
    // QModelIndex parent = sourceIndex.parent();
    // int row = sourceIndex.row();
    // if (!sourceIndex.isValid()) {
    //   return m_root;
    // }

    // check if the model index corresponds to the invisible root item in source model
    if (sourceIndex == 
            static_cast<QStandardItemModel *>(sourceModel())->invisibleRootItem()->index()) {
      qDebug() << "creating index for invisible root item "
          << static_cast<QStandardItemModel *>(sourceModel())->invisibleRootItem()->index();
      return createIndex(sourceIndex.row(), 0, sourceIndex.internalId());
    }

    // First check to see if the source index is the proxy root
    if (sourceIndex == m_root) {
      //qDebug() << "source exactly matches root already.";
      return createIndex(sourceIndex.row(), 0, sourceIndex.internalId());
    }

    // If the source index is a child of the proxy root, one if its ancestors IS proxy root
    QModelIndex ancestorIndex = sourceIndex.parent();
    QString n = "";
    //qDebug() << ancestorIndex;
    while (ancestorIndex.isValid() && ancestorIndex != m_root) {
      ancestorIndex = ancestorIndex.parent();
      n += "\t";
      //qDebug() << n << ancestorIndex;
    }
    if (ancestorIndex.isValid()) {
      return createIndex(sourceIndex.row(), 0, sourceIndex.internalId());
      // return QModelIndex(parent.child(row));
    }
    else {
      return QModelIndex();
    }
  }


  //Returns the model index in the source that corresponds to the proxy index
  //in the proxy model
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
      //qDebug() << "persistent index is valid: " << persistentIndex;
      //qDebug() << "parent index: " << persistentIndex.parent();
      m_root = persistentIndex;
    }
    else {
      //qDebug() << "persistent index NOT valid.";
      m_root = QPersistentModelIndex(QModelIndex());
    }

    // // qDebug() << "can convert to qstandarditem: " << data.canConvert<QStandardItem *>();
    // if (data.canConvert<ProjectItem *>()) {
    //  m_root = data.value<ProjectItem *>();
    // }
    QIdentityProxyModel::setSourceModel(newSourceModel);
  }


  bool SubTreeProxyModel::setRoot(const QStandardItem *item) {
    QAbstractItemModel::removeRows(1,2,item->index());

    qDebug() << "Persistent's PARENT: " << m_root.parent();
    if (m_root.isValid()) {
      qDebug() <<"m_root is valid";
      return true;
    }
    else {
      qDebug() <<"m_root is not valid";

      return false;
    }
  }


}
