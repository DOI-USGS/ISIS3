#include "IsisDebug.h"

#include "PointModel.h"

#include <iostream>

#include <QFuture>
#include <QFutureWatcher>
#include <QString>
#include <QtConcurrentMap>

#include "ControlNet.h"
#include "ControlPoint.h"

#include "FilterWidget.h"
#include "PointParentItem.h"
#include "MeasureLeafItem.h"
#include "RootItem.h"


using std::cerr;


namespace Isis
{
  PointModel::PointModel(ControlNet * controlNet, QString name, QTreeView * tv,
      QObject * parent) : TreeModel(controlNet, name, tv, parent)
  {
    watcher = new QFutureWatcher< QAtomicPointer< RootItem > >;
    connect(watcher, SIGNAL(finished()),
            this, SLOT(rebuildItemsDone()));

    rebuildItems();
  }


  PointModel::~PointModel()
  {
  }
  
  
  PointModel::CreateRootItemFunctor::CreateRootItemFunctor(
      FilterWidget * fw) : filter(fw)
  {
  }
  
  
  PointParentItem * PointModel::CreateRootItemFunctor::operator()(
      ControlPoint * const & point) const
  {
    PointParentItem * pointItem = NULL;

//     int sum1 = 1;

    if (!filter || filter->evaluate(point))
    {
      pointItem = new PointParentItem(point);
      for (int j = 0; j < point->GetNumMeasures(); j++)
      {
        const ControlMeasure * measure = point->GetMeasure(j);
        ASSERT(measure);
        if (!filter || filter->evaluate(measure))
        {
          MeasureLeafItem * measureItem = new MeasureLeafItem(
              const_cast< ControlMeasure * >(measure), pointItem);
          pointItem->addChild(measureItem);
        }
        
//         int sum = 0;
//         for (int z = 0; z < 99999999; z += 2)
//           sum += z--;
//           
//         sum1 = sum;
      }
    }
    
//     if (sum1)
      return pointItem;
  }
  
  
  void PointModel::CreateRootItemFunctor::addToRootItem(
      QAtomicPointer< RootItem > & root, PointParentItem * const & item)
  {
    if (!root)
      root = new RootItem;
    
    if (item)
      root->addChild(item);
  }


  void PointModel::rebuildItems()
  {
//     cerr << "rebuildItems called... filter: " << filter << "\n";
    
//     cerr << "  returned from clear()\n";
    QFuture< QAtomicPointer< RootItem > > futureRoot;
    if (watcher->isStarted())
    {
      futureRoot = watcher->future();
      futureRoot.cancel();
//       if (futureRoot.result())
//         delete futureRoot.result();
    }
    
    futureRoot = QtConcurrent::mappedReduced(cNet->getPoints(),
        CreateRootItemFunctor(filter),
        &CreateRootItemFunctor::addToRootItem,
        QtConcurrent::OrderedReduce | QtConcurrent::SequentialReduce);

    watcher->setFuture(futureRoot);
  }
  
  
  void PointModel::rebuildItemsDone()
  {
   
    saveViewState();
    
    clear();
    QAtomicPointer< RootItem > newRoot = watcher->future();

    if (newRoot->childCount())
    {
      beginInsertRows(QModelIndex(), 0, newRoot->childCount() - 1);
      ASSERT(rootItem);
      delete rootItem;
      rootItem = NULL;
      rootItem = newRoot;
      endInsertRows();
    }
    else
    {
      ASSERT(newRoot);
      delete newRoot;
      newRoot = NULL;
    }

    loadViewState();
  }
}
