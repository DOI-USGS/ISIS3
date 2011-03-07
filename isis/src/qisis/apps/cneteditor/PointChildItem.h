#ifndef PointChildItem_H
#define PointChildItem_H


#include "TreeItem.h"


class QVariant;

namespace Isis
{
  class ControlPoint;

  class PointChildItem : public TreeItem
  {
    public:
      PointChildItem(Isis::ControlPoint * cp,
          TreeItem * parent = 0);
      virtual ~PointChildItem();

      void addChild(TreeItem * child);
      void removeChild(int row);
      int columnCount() const;
      QVariant data(int column) const;
      void setData(int column, const QVariant & value);
      void deleteSource();
      TreeItem::InternalPointerType pointerType() const;

    private: // Disallow copying of this class
      PointChildItem(const PointChildItem & other);
      const PointChildItem & operator=(const PointChildItem & other);


    private:
      Isis::ControlPoint * point;
  };
}

#endif
