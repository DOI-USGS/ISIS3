#ifndef ImageParentItem_H
#define ImageParentItem_H

#include <QString>

#include "AbstractImageItem.h"
#include "AbstractParentItem.h"


namespace Isis {
  class ControlNet;

  /**
   * @brief Tree item that is a parent and represents an image
   *
   * This class represents a parent item in a tree structure that holds
   * serial number data.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   */
  class ImageParentItem : public AbstractImageItem, public AbstractParentItem {
    public:
      ImageParentItem(QString imageSerial, ControlNet *net,
                      int avgCharWidth, AbstractTreeItem *parent = 0);
      virtual ~ImageParentItem();


    private: // Disallow copying of this class
      ImageParentItem(const ImageParentItem &other);
      const ImageParentItem &operator=(const ImageParentItem &other);
  };
}

#endif
