#include "IsisDebug.h"

#include "SerialModel.h"

#include <QList>
#include <QModelIndex>
#include <QString>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"

#include "SerialParentItem.h"
#include "PointLeafItem.h"


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


  void SerialModel::rebuildItems()
  {
    clearParentItems();

    QList< ControlCubeGraphNode * > nodes = cNet->GetCubeGraphNodes();
    beginInsertRows(QModelIndex(), 0, nodes.size() - 1);
    for (int i = 0; i < nodes.size(); i++)
    {
      ControlCubeGraphNode * node = nodes[i];
      SerialParentItem * serialItem = new SerialParentItem(node);
//       serialItem->setRow(i);
      parentItems->append(serialItem);

      QList< ControlMeasure * > measures = node->getMeasures();
      for (int j = 0; j < measures.size(); j++)
      {
        ControlPoint * point = measures[j]->Parent();
        PointLeafItem * pointItem = new PointLeafItem(point, serialItem);
        serialItem->addChild(pointItem);
      }
    }
    endInsertRows();

//     emit (dataChanged(QModelIndex(), QModelIndex()));
  }
}
