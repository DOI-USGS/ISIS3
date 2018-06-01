#include "IsisDebug.h"

#include "ImageParentItem.h"


namespace Isis {
  ImageParentItem::ImageParentItem(QString imageSerial,
      int avgCharWidth, AbstractTreeItem *parent)
    : AbstractTreeItem(parent), AbstractImageItem(imageSerial, avgCharWidth) {
  }


  ImageParentItem::~ImageParentItem() {
  }
}
