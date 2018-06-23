#include "IsisDebug.h"

#include "ImageParentItem.h"

#include "ControlNet.h"

namespace Isis {
  ImageParentItem::ImageParentItem(QString imageSerial, ControlNet *net,
                                   int avgCharWidth, AbstractTreeItem *parent)
        : AbstractTreeItem(parent), AbstractImageItem(imageSerial, net, avgCharWidth) {
  }


  ImageParentItem::~ImageParentItem() {
  }
}
