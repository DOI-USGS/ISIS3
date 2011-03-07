#ifndef SerialConnectionParentItem_H
#define SerialConnectionParentItem_H


#include "TreeItem.h"


class QVariant;

namespace Isis
{
  class ControlCubeGraphNode;

  class SerialConnectionParentItem : public TreeItem
  {
    public:
      SerialConnectionParentItem(Isis::ControlCubeGraphNode * cubeGraphNode,
          TreeItem * parent = 0);
      virtual ~SerialConnectionParentItem();

      void addChild(TreeItem * child);
      void removeChild(int row);
      int columnCount() const;
      QVariant data(int column) const;
      void setData(int column, const QVariant & value);
      void deleteSource();
      TreeItem::InternalPointerType pointerType() const;


    private: // Disallow copying of this class
      SerialConnectionParentItem(const SerialConnectionParentItem & other);
      const SerialConnectionParentItem & operator=(const SerialConnectionParentItem & other);


    private:
      Isis::ControlCubeGraphNode * ccgn;
  };
}

#endif
