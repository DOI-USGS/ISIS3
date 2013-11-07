#ifndef ConnectionParentItem_H
#define ConnectionParentItem_H


#include "AbstractImageItem.h"
#include "AbstractParentItem.h"


namespace Isis {
  class ControlCubeGraphNode;

  namespace CnetViz {

    /**
     * @brief Tree item that is a parent and represents an image
     *
     * This class represents a parent item in a tree structure that holds
     * serial number data.
     *
     * @author ????-??-?? Eric Hyer
     *
     * @internal
     */
    class ConnectionParentItem : public AbstractImageItem,
      public AbstractParentItem {
      public:
        ConnectionParentItem(ControlCubeGraphNode *node,
            int avgCharWidth, AbstractTreeItem *parent = 0);
        virtual ~ConnectionParentItem();

        void addChild(AbstractTreeItem *child);


      private: // Disallow copying of this class
        ConnectionParentItem(const ConnectionParentItem &);
        const ConnectionParentItem &operator=(const ConnectionParentItem &);
    };
  }
}

#endif
