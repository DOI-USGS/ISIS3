#ifndef ConnectionParentItem_H
#define ConnectionParentItem_H


#include "AbstractSerialItem.h"
#include "AbstractParentItem.h"


namespace Isis
{
  class ControlCubeGraphNode;

  class ConnectionParentItem : public AbstractSerialItem,
    public AbstractParentItem
  {
    public:
      ConnectionParentItem(ControlCubeGraphNode * node,
          int avgCharWidth, AbstractTreeItem * parent = 0);
      virtual ~ConnectionParentItem();

      void addChild(AbstractTreeItem * child);


    private: // Disallow copying of this class
      ConnectionParentItem(const ConnectionParentItem &);
      const ConnectionParentItem & operator=(const ConnectionParentItem &);
  };
}

#endif
