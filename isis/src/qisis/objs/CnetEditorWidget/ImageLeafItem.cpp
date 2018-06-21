#include "IsisDebug.h"

#include "ImageLeafItem.h"


namespace Isis {
  ImageLeafItem::ImageLeafItem(QString imageSerial, ControlNet *net,
                               int avgCharWidth, AbstractTreeItem *parent)
        : AbstractTreeItem(parent), AbstractImageItem(imageSerial, net, avgCharWidth) {
  }


  ImageLeafItem::~ImageLeafItem() {
  }
}
