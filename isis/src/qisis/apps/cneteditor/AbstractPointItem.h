#ifndef AbstractPointItem_H
#define AbstractPointItem_H

#include "AbstractTreeItem.h"


class QString;


namespace Isis
{
  class ControlPoint;

  class AbstractPointItem : public virtual AbstractTreeItem
  {
    public:
      AbstractPointItem(Isis::ControlPoint * cp, int avgCharWidth,
          AbstractTreeItem * parent = 0);
      virtual ~AbstractPointItem();

      QString getData() const;
      void deleteSource();
      InternalPointerType getPointerType() const;
      void * getPointer() const;
      bool hasPoint(ControlPoint *) const;


    private: // disable copying of this class
      AbstractPointItem(const AbstractPointItem & other);
      const AbstractPointItem & operator=(const AbstractPointItem & other);


    private:
      ControlPoint * point;
  };
}

#endif
