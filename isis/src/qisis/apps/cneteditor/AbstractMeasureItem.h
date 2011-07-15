#ifndef AbstractMeasureItem_H
#define AbstractMeasureItem_H


#include "AbstractTreeItem.h"


class QString;


namespace Isis
{
  class ControlMeasure;

  class AbstractMeasureItem : public virtual AbstractTreeItem
  {
    public:
      AbstractMeasureItem(ControlMeasure * cm, int avgCharWidth,
          AbstractTreeItem * parent = 0);
      virtual ~AbstractMeasureItem();

      QString getData() const;
      void deleteSource();
      InternalPointerType getPointerType() const;
      void * getPointer() const;
      bool hasMeasure(ControlMeasure *) const;


    private: // disable copying of this class
      AbstractMeasureItem(const AbstractMeasureItem & other);
      const AbstractMeasureItem & operator=(const AbstractMeasureItem & other);


    private:
      ControlMeasure * measure;
  };
}

#endif
