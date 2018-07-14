#include "SortFilterProxyModel.h"

#include <QAbstractItemModel>
#include <QColor>
#include <QBrush>
#include <QIdentityProxyModel>
#include <QModelIndex>
#include <QObject>
#include <QPersistentModelIndex>
#include <QSet>
#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QStandardItem>
#include <QUuid>
#include <QVariant>

#include "BundleObservationSolveSettings.h"
#include "BundleSettings.h"
#include "Camera.h"
#include "Cube.h"
#include "Image.h"
#include "ImageList.h"
#include "ProjectItem.h"
#include "ProjectItemModel.h"

namespace Isis {

  SortFilterProxyModel::SortFilterProxyModel(QObject *parent) :
      QSortFilterProxyModel(parent) {
  }


  bool SortFilterProxyModel::lessThan(const QModelIndex &source_left,
                                      const QModelIndex &source_right) const {

    QString leftBOSS("%");
    QString rightBOSS("%");
    ProjectItem * left = baseModel->itemFromIndex(source_left );
    ProjectItem * right = baseModel->itemFromIndex(source_right );


    if (left->isImage() && right->isImage() ) {
      Image * leftImg = left->data().value<Image *>();
      Image * rightImg = right->data().value<Image *>();

      foreach(QUuid * key,m_bossMap.keys() ) {

        QSet<QString> obsNumbers = m_bossMap[key];
        if (obsNumbers.contains(leftImg->observationNumber())) {
          leftBOSS = key->toString();
        }
        if (obsNumbers.contains(rightImg->observationNumber())) {
          rightBOSS = key->toString();
        }
      }

    }

    return (leftBOSS < rightBOSS);

  }

  QModelIndex SortFilterProxyModel::index(int row, int column, const QModelIndex & parent ) const {



    //qDebug() << "columnCount -1 = " << columnCount() -1 << "  column = " << column;
    if (column > 0 ) {
      //qDebug() << "parent child = " << parent.child(row,0).data(Qt::DisplayRole).toString();
      //QModelIndex ix = createIndex(row,column,parent.internalId());
      return parent.child(row,0);
      //return createIndex(row,column,parent.child(row,0).internalId() );

      //return ix;

    }
    else
      return QSortFilterProxyModel::index(row,column,parent);

  }


  QModelIndex SortFilterProxyModel::mapToSource(const QModelIndex &proxyIndex) const {
    if (proxyIndex.column() > 0) {
      return QModelIndex();

    }
    else {
      return QSortFilterProxyModel::mapToSource(proxyIndex);

    }

  }

  QModelIndex SortFilterProxyModel::parent(const QModelIndex & child) const {


    if (child.column() > 0) {

      return QModelIndex();


    }
    else {

      return QSortFilterProxyModel::parent(child);
    }

  }


  QVariant SortFilterProxyModel::headerData(int section, Qt::Orientation orientation, int role) const{

    if (role != Qt::DisplayRole)
      return QVariant();

    if (orientation == Qt::Horizontal) {

      switch (section) {

       case 0:
        return tr("Image ID");
      case 1:
        return tr("BOSS");
      default:
        return QVariant();

      }

    }

  return section +1;

  }

  void SortFilterProxyModel::setSelectedItems(QList<ProjectItem *> selected,QSharedPointer<BundleSettings> bundleQSP){

    m_bossList = bundleQSP->observationSolveSettings();
    QList<QModelIndex> selIx;
    foreach(ProjectItem * item,selected) {
      int col = item->index().column();
      if (col == 0) {
        selIx.append(item->index() );
      }

    }

    foreach (QModelIndex ix, selIx) {

      if (ix.isValid() ) {

      ProjectItem * projItem = baseModel->itemFromIndex(ix);
      if (projItem) {
        if (projItem->isImage() ) {
          Image * img = projItem->data().value<Image *>();
          QString instrumentName = img->cube()->camera()->instrumentNameShort();
          int cameraType = img->cube()->camera()->GetCameraType();
          QPair<QString,int> node;
          node.first = instrumentName;
          node.second = cameraType;
          images[ix]=node;
        }
      }

      QList<BundleObservationSolveSettings> bossList = bundleQSP->observationSolveSettings();

      foreach(BundleObservationSolveSettings boss, bossList) {
            m_bossMap[boss.uuid()] = boss.observationNumbers();
      }


#if 0
        if(projItem->isImageList() ) {
          ImageList *imgList = projItem->data().value<ImageList *>();
        }
#endif
      }

    }

    selectedIndices=selIx;

  }

  int SortFilterProxyModel::columnCount(const QModelIndex & parent) const {
#if 0
    if (parent.isValid()) {
    int modelColumns = baseModel->columnCount(mapToSource(parent));

    if (parent.isValid() ) {

      ProjectItem *item = baseModel->itemFromIndex(mapToSource(parent) );
      if (item->isImageList() || item->isImage() ) {
        createIndex(parent.row(),1,parent.internalId());
        return 2;
      }


    }

     return modelColumns+1;

    }
     #endif
    //qDebug() << "Num Columns = "<< modelColumns+1;
  return 2;
  }






  QVariant SortFilterProxyModel::data(const QModelIndex & ix, int role) const  {
    QVariant v;


    if (ix.isValid()  && ix.column() == 0) {

      ProjectItem *item = baseModel->itemFromIndex(mapToSource(ix) );
        if (item->isImage() && role==Qt::DisplayRole) {

          Image * img = item->data().value<Image *>();
           return item->text();
          }


      }

    if ( ix.isValid() && ix.column() ==1 ) {

      QModelIndex siblingix = ix.sibling(ix.row(),0);

      ProjectItem *item = baseModel->itemFromIndex(mapToSource(siblingix) );
      if (item) {
        if (item->isImage())
          return item->text();

      }


      //QBrush bkgrnd(QColor(Qt::green));
      return v;

    }


    return v;
  }


  bool SortFilterProxyModel::setData(const QModelIndex & ix, const QVariant & value, int role) const {

    bool accept(false);
    if (ix.isValid() ) {
      ProjectItem *item = baseModel->itemFromIndex(mapToSource(ix) );
        if (role==Qt::BackgroundRole) {
          qDebug() << "SHAPOOPY!";
          accept = true;
          }
        }
    else {
      qDebug() << "ix is not valid.";
    }

    return accept;
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
#if 0
  QModelIndex SortFilterProxyModel::mapFromSource(const QModelIndex &sourceIndex) const {

    //QModelIndex ix = createIndex(sourceIndex.row(),sourceIndex.column(),  static_cast<QStandardItemModel *>(sourceModel())->invisibleRootItem()->index().internalId());

    QModelIndex ix = createIndex(sourceIndex.row(),0,m_root.internalId());
    qDebug() << "imgRoot in Proxy:  " << ix  << ":" << ix.isValid();

    return ix;



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
#endif

  bool SortFilterProxyModel::filterAcceptsRow(int sourceRow,
                                                        const QModelIndex &sourceParent) const {

    //qDebug() << "SourceParent:  " << sourceParent;
    //qDebug() << "SourceParent is valid = " << sourceParent.isValid();

#if 0
    if (sourceParent.isValid() ) {
      qDebug() << sourceParent.data(0).toString();
      return true;
    }

    else {

      qDebug() << "sourceParentIsInvalid:" << sourceParent;
      return false;
    }


      return false;
    bool accept(true);
#endif
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




}//end namespace


