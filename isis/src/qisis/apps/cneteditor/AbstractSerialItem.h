#ifndef AbstractSerialItem_H
#define AbstractSerialItem_H


#include "AbstractTreeItem.h"


class QString;


namespace Isis
{
  class ControlCubeGraphNode;

  class AbstractSerialItem : public virtual AbstractTreeItem
  {
    public:
      AbstractSerialItem(ControlCubeGraphNode * cubeGraphNode,
          int avgCharWidth, AbstractTreeItem * parent = 0);
      virtual ~AbstractSerialItem();

      QString getData() const;
      void deleteSource();
      InternalPointerType getPointerType() const;
      void * getPointer() const;
      bool hasNode(ControlCubeGraphNode *) const;


    private: // disable copying of this class
      AbstractSerialItem(const AbstractSerialItem & other);
      const AbstractSerialItem & operator=(const AbstractSerialItem & other);


    private:
      ControlCubeGraphNode * ccgn;
  };
}

#endif
