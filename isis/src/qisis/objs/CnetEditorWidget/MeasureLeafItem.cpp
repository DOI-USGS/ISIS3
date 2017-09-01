#include "IsisDebug.h"

#include "MeasureLeafItem.h"


namespace Isis {
  MeasureLeafItem::MeasureLeafItem(ControlMeasure *cm,
      int avgCharWidth, AbstractTreeItem *parent)
    : AbstractTreeItem(parent), AbstractMeasureItem(cm, avgCharWidth) {
  }


  MeasureLeafItem::~MeasureLeafItem() {
  }
}
