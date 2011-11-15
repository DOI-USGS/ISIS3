#ifndef ImageLeafItem_H
#define ImageLeafItem_H


#include "AbstractImageItem.h"
#include "AbstractLeafItem.h"


namespace Isis
{
  class ControlCubeGraphNode;

  namespace CnetViz
  {
    class ImageLeafItem : public AbstractImageItem, public AbstractLeafItem
    {
      public:
        ImageLeafItem(ControlCubeGraphNode * node,
            int avgCharWidth, AbstractTreeItem * parent = 0);
        virtual ~ImageLeafItem();


      private: // Disallow copying of this class
        ImageLeafItem(const ImageLeafItem & other);
        const ImageLeafItem & operator=(const ImageLeafItem & other);
    };
  }
}

#endif
