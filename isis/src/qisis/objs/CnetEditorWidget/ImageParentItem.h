#ifndef ImageParentItem_H
#define ImageParentItem_H


#include "AbstractImageItem.h"
#include "AbstractParentItem.h"


namespace Isis {
  class ControlCubeGraphNode;

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
   */
  class ImageParentItem : public AbstractImageItem, public AbstractParentItem {
    public:
      ImageParentItem(ControlCubeGraphNode *node,
          int avgCharWidth, AbstractTreeItem *parent = 0);
      virtual ~ImageParentItem();


    private: // Disallow copying of this class
      ImageParentItem(const ImageParentItem &other);
      const ImageParentItem &operator=(const ImageParentItem &other);
  };
}

#endif
