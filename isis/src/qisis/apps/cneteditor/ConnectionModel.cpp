#include "IsisDebug.h"

#include "ConnectionModel.h"

#include <iostream>

#include <QFuture>
#include <QFutureWatcher>
#include <QList>
#include <QString>
#include <QtConcurrentMap>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlNet.h"

#include "ConnectionParentItem.h"
#include "FilterWidget.h"
#include "PointLeafItem.h"
#include "RootItem.h"
#include "SerialParentItem.h"


using std::cerr;


namespace Isis
{
  ConnectionModel::ConnectionModel(ControlNet * cNet, QString name,
      QTreeView * tv, QObject * parent) : TreeModel(cNet, name, tv, parent)
  {
    watcher = new QFutureWatcher< QAtomicPointer< RootItem > >;
    connect(watcher, SIGNAL(finished()),
            this, SLOT(rebuildItemsDone()));

    rebuildItems();
  }


  ConnectionModel::~ConnectionModel()
  {
  }
  
  
  ConnectionModel::CreateRootItemFunctor::CreateRootItemFunctor(
      FilterWidget * fw) : filter(fw)
  {
  }
  
  
  ConnectionParentItem * ConnectionModel::CreateRootItemFunctor::operator()(
      ControlCubeGraphNode * const & node) const
  {
    ConnectionParentItem * parentItem = new ConnectionParentItem(node);

    QList< ControlCubeGraphNode * > connectedNodes = node->getAdjacentNodes();
    for (int j = 0; j < connectedNodes.size(); j++)
    {
      ControlCubeGraphNode * connectedNode = connectedNodes[j];
      SerialParentItem * serialItem = new SerialParentItem(connectedNode,
          parentItem);

      QList< ControlMeasure * > measures = connectedNode->getMeasures();
      for (int k = 0; k < measures.size(); k++)
      {
        ControlPoint * point = measures[k]->Parent();
        PointLeafItem * pointItem = new PointLeafItem(point, serialItem);
        serialItem->addChild(pointItem);
      }

      parentItem->addChild(serialItem);
    }
    
    return parentItem;
  }
  
  
  void ConnectionModel::CreateRootItemFunctor::addToRootItem(
      QAtomicPointer< RootItem > & root, ConnectionParentItem * const & item)
  {
    if (!root)
      root = new RootItem;
    
    if (item)
      root->addChild(item);
  }


  void ConnectionModel::rebuildItems()
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
  
  
  void ConnectionModel::rebuildItemsDone()
  {
    saveViewState();
    
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

    loadViewState();
  }
}
