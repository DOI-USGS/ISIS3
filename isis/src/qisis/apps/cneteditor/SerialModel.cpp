#include "IsisDebug.h"

#include "SerialModel.h"

#include <QFuture>
#include <QList>
#include <QModelIndex>
#include <QString>
#include <QtConcurrentMap>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"

#include "FilterWidget.h"
#include "PointLeafItem.h"
#include "RootItem.h"
#include "SerialParentItem.h"


namespace Isis
{
  SerialModel::SerialModel(ControlNet * controlNet, QString name,
      QTreeView * tv, QObject * parent) : TreeModel(controlNet, name, tv,
            parent)
  {
    rebuildItems();
  }


  SerialModel::~SerialModel()
  {
  }


  bool SerialModel::CreateRootItemFunctor::rootInstantiated = false;
  
  
  SerialModel::CreateRootItemFunctor::CreateRootItemFunctor(
      FilterWidget * fw) : filter(fw)
  {
    rootInstantiated = false;
  }
  
  
  SerialParentItem * SerialModel::CreateRootItemFunctor::operator()(
      ControlCubeGraphNode * const & node) const
  {
    SerialParentItem * serialItem = NULL;

    if (!filter || filter->evaluate(node))
    {
      serialItem = new SerialParentItem(node);
      QList< ControlMeasure * > measures = node->getMeasures();
      for (int j = 0; j < measures.size(); j++)
      {
        ControlPoint * point = measures[j]->Parent();
        ASSERT(measure);
        if (!filter || filter->evaluate(point))
        {
          PointLeafItem * pointItem = new PointLeafItem(
              point, serialItem);
          serialItem->addChild(pointItem);
        }
      }
    }
    
    return serialItem;
  }
  
  
  void SerialModel::CreateRootItemFunctor::addToRootItem(RootItem *& root,
      SerialParentItem * const & item)
  {
    if (!rootInstantiated)
    {
      root = new RootItem;
      rootInstantiated = true;
    }
    
    if (item)
      root->addChild(item);
  }


  void SerialModel::rebuildItems()
  {
    clear();
    
    QFuture< RootItem * > futureRoot = QtConcurrent::mappedReduced(
        cNet->GetCubeGraphNodes(), CreateRootItemFunctor(filter),
        &CreateRootItemFunctor::addToRootItem,
        QtConcurrent::OrderedReduce | QtConcurrent::SequentialReduce);
        
    RootItem * newRoot = futureRoot;
    
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
  }
}
