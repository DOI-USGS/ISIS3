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
       //qDebug() << "persistent index is valid: " << persistentIndex;
       m_root = persistentIndex;
     }
     else {
       //qDebug() << "persistent index NOT valid.";
       m_root = QPersistentModelIndex(QModelIndex());
     }

     baseModel = newSourceModel;
     QSortFilterProxyModel::setSourceModel(newSourceModel);
   }


  bool SortFilterProxyModel::setRoot(const QStandardItem *item) {

    m_root = QPersistentModelIndex(item->index());   
    return true;


  }


  bool SortFilterProxyModel::filterAcceptsRow(int sourceRow,
                                                        const QModelIndex &sourceParent) const {
    bool accept(false);

    if (selectedIndices.count() == 0) {
      accept = true;
    }

    if (this->sourceModel()!=nullptr) {
       QModelIndex ix = this->sourceModel()->index( sourceRow, 0, sourceParent );
       if (ix.isValid() ) {
        ProjectItem * item = baseModel->itemFromIndex(ix);
        if (selectedIndices.contains(ix)  ) {
           accept = true;
         }         
        if (item->text() == "Images" ) {
          accept = true;
        }
     }//end if (ix.isValid() )

  }
  return accept;

 }


  QVariant SortFilterProxyModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();

    if (role == Qt::DisplayRole) { 
      return QSortFilterProxyModel::data(index, role);
    }
    if (role == Qt::ForegroundRole) {
      return QSortFilterProxyModel::data(index, role);
    }
    if (role == Qt::BackgroundRole) {
      // if (sourceModel()->data(mapToSource(index), Qt::UserRole+10).toBool()) {

        QColor solveOptionColor = sourceModel()->data(mapToSource(index), Qt::UserRole+10).value<QColor>();
        if (solveOptionColor.isValid()) {
          return QVariant(solveOptionColor);
        }
        else {
          return QVariant(QColor(Qt::white));
        }
      // }
      // else {
        // return sourceModel()->data(mapToSource(index), Qt::BackgroundRole);
      // }
    }
    // if (role == Qt::UserRole + 10) {
    //   return sourceModel()->data(mapToSource(index), Qt::UserRole+10).value<QColor>();
    // }
    else {
      return QVariant(); //QSortFilterProxyModel::data(index, role);
    }
  }




}//end namespace


