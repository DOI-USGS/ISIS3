#ifndef ImageLeafItem_H
#define ImageLeafItem_H


#include "AbstractImageItem.h"
#include "AbstractLeafItem.h"


namespace Isis {
  class ControlCubeGraphNode;

  /**
   * @brief Tree item that is a leaf and represents an image
   *
   * This class represents a leaf item in a tree structure that holds serial
   * number data.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal 
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054. 
   */
  class ImageLeafItem : public AbstractImageItem, public AbstractLeafItem {
    public:
      ImageLeafItem(ControlCubeGraphNode *node,
          int avgCharWidth, AbstractTreeItem *parent = 0);
      virtual ~ImageLeafItem();


    private: // Disallow copying of this class
      ImageLeafItem(const ImageLeafItem &other);
      const ImageLeafItem &operator=(const ImageLeafItem &other);
  };
}

#endif
