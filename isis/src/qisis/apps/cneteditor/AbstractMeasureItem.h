#ifndef AbstractMeasureItem_H
#define AbstractMeasureItem_H


#include "AbstractTreeItem.h"


class QVariant;


namespace Isis
{
  class ControlMeasure;

  class AbstractMeasureItem : public virtual AbstractTreeItem
  {
    public:
      AbstractMeasureItem(Isis::ControlMeasure * cm,
          AbstractTreeItem * parent = 0);
      virtual ~AbstractMeasureItem();

      QVariant data() const;
      void deleteSource();
      InternalPointerType pointerType() const;
      bool hasMeasure(ControlMeasure *) const;


    private: // disable copying of this class
      AbstractMeasureItem(const AbstractMeasureItem & other);
      const AbstractMeasureItem & operator=(const AbstractMeasureItem & other);


    private:
      Isis::ControlMeasure * measure;
  };
}

#endif
