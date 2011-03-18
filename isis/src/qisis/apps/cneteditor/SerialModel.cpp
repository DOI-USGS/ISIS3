#include "IsisDebug.h"

#include "SerialModel.h"

#include <QList>
#include <QString>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"

#include "SerialParentItem.h"
#include "PointChildItem.h"


namespace Isis
{
  SerialModel::SerialModel(ControlNet * controlNet, QString name,
      QObject * parent) : TreeModel(controlNet, name, parent)
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
      parentItems->append(serialItem);

      QList< ControlMeasure * > measures = node->getMeasures();
      for (int j = 0; j < measures.size(); j++)
      {
        ControlPoint * point = measures[j]->Parent();
        PointChildItem * pointItem = new PointChildItem(point, serialItem);
        serialItem->addChild(pointItem);
      }
    }
    endInsertRows();
  }
}
