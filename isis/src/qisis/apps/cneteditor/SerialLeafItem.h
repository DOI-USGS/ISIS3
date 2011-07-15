#ifndef SerialLeafItem_H
#define SerialLeafItem_H


#include "AbstractSerialItem.h"
#include "AbstractLeafItem.h"


namespace Isis
{
  class ControlCubeGraphNode;

  class SerialLeafItem : public AbstractSerialItem, public AbstractLeafItem
  {
    public:
      SerialLeafItem(Isis::ControlCubeGraphNode * node,
          int avgCharWidth, AbstractTreeItem * parent = 0);
      virtual ~SerialLeafItem();


    private: // Disallow copying of this class
      SerialLeafItem(const SerialLeafItem & other);
      const SerialLeafItem & operator=(const SerialLeafItem & other);
  };
}

#endif
