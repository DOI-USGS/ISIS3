#ifndef PointParentItem_H
#define PointParentItem_H


#include "TreeItem.h"


class QVariant;

namespace Isis
{
  class ControlPoint;

  class PointParentItem : public TreeItem
  {
    public:
      PointParentItem(Isis::ControlPoint * cp,
          TreeItem * parent = 0);
      virtual ~PointParentItem();

      void addChild(TreeItem * child);
      void removeChild(int row);
      int columnCount() const;
      QVariant data(int column) const;
      void setData(int column, const QVariant & value);
      void deleteSource();
      TreeItem::InternalPointerType pointerType() const;

    private: // Disallow copying of this class
      PointParentItem(const PointParentItem & other);
      const PointParentItem & operator=(const PointParentItem & other);


    private:
      Isis::ControlPoint * point;
  };
}

#endif
