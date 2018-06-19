#include "SortFilterProxyModel.h"

#include <QAbstractItemModel>
#include <QIdentityProxyModel>
#include <QModelIndex>
#include <QObject>
#include <QPersistentModelIndex>
#include <QSortFilterProxyModel>
#include <QStandardItem>
#include <QVariant>

#include "ProjectItem.h"
#include "ProjectItemModel.h"

namespace Isis {

  SortFilterProxyModel::SortFilterProxyModel(QObject *parent) :
      QSortFilterProxyModel(parent) {
  }


  void SortFilterProxyModel::setSelectedItems(QList<ProjectItem *> selected){

    QList<QModelIndex> selIx;
    foreach(ProjectItem * item,selected) {
      selIx.append(item->index() );

    }

    foreach(QModelIndex ix,selIx) {
      selectedIndexRows.append(ix.row() );
    }

    selectedIndices=selIx;

  }

  void SortFilterProxyModel::setSourceModel(ProjectItemModel *newSourceModel) {
     
     QPersistentModelIndex persistentIndex(newSourceModel->index(0,0,QModelIndex()));
    


     if (persistentIndex.isValid()) {
       qDebug() << "persistent index is valid: " << persistentIndex;
      
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
     QSortFilterProxyModel::setSourceModel(newSourceModel);
   }




  bool SortFilterProxyModel::setRoot(const QStandardItem *item) {

    m_root = QPersistentModelIndex(item->index());
    qDebug() << "Setting m_root to:  " << m_root.data(0).toString();
    //root = item->index();
    return true;

    //qDebug() << "m_root = " << m_root;

    //if (m_root.isValid()) {
    //  qDebug() <<"m_root is valid";
    //  return true;
   // }
    //else {
      //qDebug() <<"m_root is not valid";

      //return false;
    //}
  }


  bool SortFilterProxyModel::filterAcceptsRow(int sourceRow,
                                                        const QModelIndex &sourceParent) const {

    static QList<QModelIndex> accepted = selectedIndices;

    qDebug() << "filterAcceptsRow";
    bool accept(false);

    if (this->sourceModel()!=nullptr) {

       QModelIndex ix = this->sourceModel()->index( sourceRow, 0, sourceParent );
       if (ix.isValid() ) {
         if (this->sourceModel()->hasChildren(ix) ) {
           qDebug() << "Has children:  " << ix.data(0).toString();
           accept = true;
         }
         if (accepted.contains(ix)  && (this->sourceModel()->hasChildren(ix))) {
           qDebug() << "Accepted (has children):" << ix.data(0).toString();
           int numChildren = this->sourceModel()->rowCount(ix);
           for (int i = 0; i < numChildren;i++) {

             QModelIndex ixchild = this->sourceModel()->index(i,0,ix);
             accepted.append(ixchild);

             }
           }
         }
         if (accepted.contains(ix) ) {
           qDebug() << "Accepted:  " << ix.data(0).toString();
           accept = true;
         }

         qDebug() << "Rejected:  " << ix.data(0).toString();
       }

       return accept;

    }


}//end namespace


