#include "IsisDebug.h"

#include "PointModel.h"

#include <iostream>

#include <QFuture>
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
    rebuildItems();
  }


  PointModel::~PointModel()
  {
  }
  
  
  bool PointModel::CreateRootItemFunctor::rootInstantiated = false;
  
  
  PointModel::CreateRootItemFunctor::CreateRootItemFunctor(
      FilterWidget * fw) : filter(fw)
  {
    rootInstantiated = false;
  }
  
  
  PointParentItem * PointModel::CreateRootItemFunctor::operator()(
      ControlPoint * const & point) const
  {
    PointParentItem * pointItem = NULL;

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
      }
    }
    
    return pointItem;
  }
  
  
  void PointModel::CreateRootItemFunctor::addToRootItem(RootItem *& root,
      PointParentItem * const & item)
  {
    if (!rootInstantiated)
    {
      root = new RootItem;
      rootInstantiated = true;
    }
    
    if (item)
      root->addChild(item);
  }


  void PointModel::rebuildItems()
  {
//     cerr << "rebuildItems called... filter: " << filter << "\n";
    
    clear();
    
//     cerr << "  returned from clear()\n";
    
    QFuture< RootItem * > futureRoot = QtConcurrent::mappedReduced(
        cNet->getPoints(), CreateRootItemFunctor(filter),
        &CreateRootItemFunctor::addToRootItem,
        QtConcurrent::OrderedReduce | QtConcurrent::SequentialReduce);
        
    RootItem * newRoot = futureRoot;
    
    
    
//     RootItem * newRoot = new RootItem;
//     for (int i = 0; i < cNet->GetNumPoints(); i++)
//     {
//       ControlPoint * point = cNet->GetPoint(i);
//       if (!filter || filter->evaluate(point))
//       {
//         PointParentItem * pointItem = new PointParentItem(point);
//         newRoot->addChild(pointItem);
//         for (int j = 0; j < point->GetNumMeasures(); j++)
//         {
//           ControlMeasure * measure = point->GetMeasure(j);
//           ASSERT(measure);
//           if (!filter || filter->evaluate(measure))
//           {
//             MeasureLeafItem * measureItem = new MeasureLeafItem(
//                 measure, pointItem);
//             pointItem->addChild(measureItem);
//           }
//         }
//       }
//     }
    
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
//     cerr << "rebuildItems done\n\n";
  }
}
