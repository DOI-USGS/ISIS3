#include "IsisDebug.h"

#include "MeasureLeafItem.h"


namespace Isis
{
  MeasureLeafItem::MeasureLeafItem(ControlMeasure * cm,
      AbstractTreeItem * parent) : AbstractTreeItem(parent),
    AbstractMeasureItem(cm)
  {
  }


  MeasureLeafItem::~MeasureLeafItem()
  {
  }
}
