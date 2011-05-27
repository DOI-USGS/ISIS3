#include "IsisDebug.h"

#include "SerialModel.h"

#include <iostream>

#include <QFuture>
#include <QFutureWatcher>
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


using std::cerr;


namespace Isis
{
  SerialModel::SerialModel(ControlNet * controlNet, QString name,
      QTreeView * tv, QObject * parent) : TreeModel(controlNet, name, tv,
            parent)
  {
    watcher = new QFutureWatcher< QAtomicPointer< RootItem > >;
    connect(watcher, SIGNAL(finished()),
            this, SLOT(rebuildItemsDone()));

    rebuildItems();
  }


  SerialModel::~SerialModel()
  {
  }


  SerialModel::CreateRootItemFunctor::CreateRootItemFunctor(
      FilterWidget * fw) : filter(fw)
  {
  }
  
  
  SerialParentItem * SerialModel::CreateRootItemFunctor::operator()(
      ControlCubeGraphNode * const & node) const
  {
    SerialParentItem * serialItem = NULL;

    if (true || !filter || filter->evaluate(node))
    {
      serialItem = new SerialParentItem(node);
      QList< ControlMeasure * > measures = node->getMeasures();
      for (int j = 0; j < measures.size(); j++)
      {
        ControlPoint * point = measures[j]->Parent();
        ASSERT(measure);
        if (true || !filter || filter->evaluate(point))
        {
          PointLeafItem * pointItem = new PointLeafItem(
              point, serialItem);
          serialItem->addChild(pointItem);
        }
      }
    }
    
    return serialItem;
  }
  
  
  void SerialModel::CreateRootItemFunctor::addToRootItem(
      QAtomicPointer< RootItem > & root, SerialParentItem * const & item)
  {
    if (!root)
      root = new RootItem;
    
    if (item)
      root->addChild(item);
  }


  void SerialModel::rebuildItems()
  {
    QFuture< QAtomicPointer< RootItem > > futureRoot;
    if (watcher->isStarted())
    {
      futureRoot = watcher->future();
      futureRoot.cancel();
//       futureRoot.waitForFinished();
//       if (futureRoot.result())
//         delete futureRoot.result();
    }
    
    futureRoot = QtConcurrent::mappedReduced(cNet->GetCubeGraphNodes(),
        CreateRootItemFunctor(filter),
        &CreateRootItemFunctor::addToRootItem,
        QtConcurrent::OrderedReduce | QtConcurrent::SequentialReduce);
    
    watcher->setFuture(futureRoot);
  }
  
  
  void SerialModel::rebuildItemsDone()
  {
    if (watcher->isCanceled())
      return;
   
//     saveViewState();
    
    clear();
    QAtomicPointer< RootItem > newRoot = watcher->future();

    if (newRoot && newRoot->childCount())
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
      if (newRoot)
      {
        delete newRoot;
        newRoot = NULL;
      }
    }

//     loadViewState();
  }
}
