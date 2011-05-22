#include "IsisDebug.h"

#include "PointModel.h"

#include <iostream>

#include <QString>

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


  void PointModel::rebuildItems()
  {
//     cerr << "rebuildItems called... filter: " << filter << "\n";
    
    clear();
    
//     cerr << "  returned from clear()\n";
    
    RootItem * newRoot = new RootItem;
    for (int i = 0; i < cNet->GetNumPoints(); i++)
    {
      ControlPoint * point = cNet->GetPoint(i);
      if (!filter || filter->evaluate(point))
      {
        PointParentItem * pointItem = new PointParentItem(point);
        newRoot->addChild(pointItem);
        for (int j = 0; j < point->GetNumMeasures(); j++)
        {
          ControlMeasure * measure = point->GetMeasure(j);
          ASSERT(measure);
          if (!filter || filter->evaluate(measure))
          {
            MeasureLeafItem * measureItem = new MeasureLeafItem(
                measure, pointItem);
            pointItem->addChild(measureItem);
          }
        }
      }
    }
    
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
