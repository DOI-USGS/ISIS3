#ifndef MeasureChildItem_H
#define MeasureChildItem_H


#include "TreeItem.h"


class QVariant;

namespace Isis
{
  class ControlMeasure;

  class MeasureChildItem : public TreeItem
  {
    public:
      MeasureChildItem(Isis::ControlMeasure * cm,
          TreeItem * parent = 0);
      virtual ~MeasureChildItem();

      void addChild(TreeItem * child);
      void removeChild(int row);
      int columnCount() const;
      QVariant data(int column) const;
      void setData(int column, const QVariant & value);
      void deleteSource();
      TreeItem::InternalPointerType pointerType() const;


    private: // Disallow copying of this class
      MeasureChildItem(const MeasureChildItem & other);
      const MeasureChildItem & operator=(const MeasureChildItem & other);


    private:
//       void validateColumn(int column) const;
//       double(Isis::ControlMeasure::*cmGetter(int col) const)() const;
//       int (Isis::ControlMeasure::*cmSetter(int col))(double d);


    private:
      Isis::ControlMeasure * measure;
  };
}

#endif
