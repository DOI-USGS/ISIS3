#include "IsisDebug.h"

#include "ImageLeafItem.h"


namespace Isis {
  ImageLeafItem::ImageLeafItem(QString imageSerial,
      int avgCharWidth, AbstractTreeItem *parent)
    : AbstractTreeItem(parent), AbstractImageItem(imageSerial, avgCharWidth) {
  }


  ImageLeafItem::~ImageLeafItem() {
  }
}
