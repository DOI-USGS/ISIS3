#ifndef SerialParentItem_H
#define SerialParentItem_H


#include "AbstractSerialItem.h"
#include "AbstractParentItem.h"


namespace Isis
{
  class ControlCubeGraphNode;

  class SerialParentItem : public AbstractSerialItem, public AbstractParentItem
  {
    public:
      SerialParentItem(Isis::ControlCubeGraphNode * node,
          int avgCharWidth, AbstractTreeItem * parent = 0);
      virtual ~SerialParentItem();


    private: // Disallow copying of this class
      SerialParentItem(const SerialParentItem & other);
      const SerialParentItem & operator=(const SerialParentItem & other);
  };
}

#endif
