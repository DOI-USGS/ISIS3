#include "IsisDebug.h"

#include "PointChildItem.h"

#include <iostream>

#include <QVariant>

#include "ControlNet.h"
#include "ControlPoint.h"
#include "iException.h"
#include "iString.h"


using std::cerr;


namespace Isis
{
  PointChildItem::PointChildItem(ControlPoint * cp,
      TreeItem * parent) : TreeItem(parent)
  {
    point = cp;
    ASSERT(point);
  }


  PointChildItem::~PointChildItem()
  {
    point = NULL;
  }


  void PointChildItem::addChild(TreeItem * child)
  {
    cerr << "PointChildItem::addChild called!\n";
  }


  void PointChildItem::removeChild(int row)
  {
    cerr << "PointChildItem::removeChild called!\n";
  }


  QVariant PointChildItem::data(int column) const
  {
    ASSERT(point);
    validateColumn(column);
//     return QVariant((measure->*cmGetter(column))());

    return QVariant((QString) point->GetId());
  }


  void PointChildItem::setData(int column, const QVariant & value)
  {
    validateColumn(column);

  }


  void PointChildItem::deleteSource()
  {
    ASSERT(point);

    point->Parent()->DeletePoint(point);
    point = NULL;
  }


  TreeItem::InternalPointerType PointChildItem::pointerType() const
  {
    return TreeItem::Point;
  }

}
