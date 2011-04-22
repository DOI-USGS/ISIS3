#ifndef AbstractPointItem_H
#define AbstractPointItem_H


#include "AbstractTreeItem.h"


class QVariant;


namespace Isis
{
  class ControlPoint;

  class AbstractPointItem : public virtual AbstractTreeItem
  {
    public:
      AbstractPointItem(Isis::ControlPoint * cp,
          AbstractTreeItem * parent = 0);
      virtual ~AbstractPointItem();

      QVariant data() const;
      void deleteSource();
      InternalPointerType pointerType() const;
      bool hasPoint(ControlPoint *) const;


    private: // disable copying of this class
      AbstractPointItem(const AbstractPointItem & other);
      const AbstractPointItem & operator=(const AbstractPointItem & other);


    private:
      Isis::ControlPoint * point;
  };
}

#endif
