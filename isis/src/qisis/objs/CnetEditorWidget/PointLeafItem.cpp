#include "IsisDebug.h"

#include "PointLeafItem.h"


namespace Isis {
  namespace CnetViz {
    PointLeafItem::PointLeafItem(ControlPoint *cp, int avgCharWidth,
        AbstractTreeItem *parent) : AbstractTreeItem(parent),
      AbstractPointItem(cp, avgCharWidth) {
    }


    PointLeafItem::~PointLeafItem() {
    }
  }
}
