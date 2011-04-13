#include "IsisDebug.h"

#include "ConnectionModel.h"

#include <QList>
#include <QString>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlNet.h"

#include "ConnectionParentItem.h"
#include "RootItem.h"
#include "SerialParentItem.h"
#include "PointLeafItem.h"


namespace Isis
{
  ConnectionModel::ConnectionModel(ControlNet * controlNet, QString name,
      QTreeView * tv, QObject * parent) : TreeModel(controlNet, name, tv, parent)
  {
    rebuildItems();
  }


  ConnectionModel::~ConnectionModel()
  {
  }


  void ConnectionModel::rebuildItems()
  {
    clear();

    QList< ControlCubeGraphNode * > nodes = cNet->GetCubeGraphNodes();
    beginInsertRows(QModelIndex(), 0, nodes.size() - 1);
    for (int i = 0; i < nodes.size(); i++)
    {
      ControlCubeGraphNode * node = nodes[i];
      ConnectionParentItem * parentItem = new ConnectionParentItem(node);
      rootItem->addChild(parentItem);

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
    }
    endInsertRows();
  }
}
