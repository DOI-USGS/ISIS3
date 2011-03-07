#ifndef SerialParentItem_H
#define SerialParentItem_H


#include "TreeItem.h"


class QVariant;

namespace Isis
{
  class ControlCubeGraphNode;

  class SerialParentItem : public TreeItem
  {
    public:
      SerialParentItem(Isis::ControlCubeGraphNode * cubeGraphNode,
          TreeItem * parent = 0);
      virtual ~SerialParentItem();

      void addChild(TreeItem * child);
      void removeChild(int row);
      int columnCount() const;
      QVariant data(int column) const;
      void setData(int column, const QVariant & value);
      void deleteSource();
      TreeItem::InternalPointerType pointerType() const;


    private: // Disallow copying of this class
      SerialParentItem(const SerialParentItem & other);
      const SerialParentItem & operator=(const SerialParentItem & other);


    private:
      Isis::ControlCubeGraphNode * ccgn;
  };
}

#endif
