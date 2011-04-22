#ifndef AbstractSerialItem_H
#define AbstractSerialItem_H


#include "AbstractTreeItem.h"


class QVariant;


namespace Isis
{
  class ControlCubeGraphNode;

  class AbstractSerialItem : public virtual AbstractTreeItem
  {
    public:
      AbstractSerialItem(Isis::ControlCubeGraphNode * cubeGraphNode,
          AbstractTreeItem * parent = 0);
      virtual ~AbstractSerialItem();

      QVariant data() const;
      void deleteSource();
      InternalPointerType pointerType() const;
      bool hasNode(ControlCubeGraphNode *) const;


    private: // disable copying of this class
      AbstractSerialItem(const AbstractSerialItem & other);
      const AbstractSerialItem & operator=(const AbstractSerialItem & other);


    private:
      Isis::ControlCubeGraphNode * ccgn;
  };
}

#endif
