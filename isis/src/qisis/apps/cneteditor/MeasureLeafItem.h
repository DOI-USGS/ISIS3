#ifndef MeasureLeafItem_H
#define MeasureLeafItem_H


#include "AbstractMeasureItem.h"
#include "AbstractLeafItem.h"


class QVariant;

namespace Isis
{
  class ControlMeasure;

  class MeasureLeafItem : public AbstractMeasureItem, public AbstractLeafItem
  {
    public:
      MeasureLeafItem(ControlMeasure * cm, AbstractTreeItem * parent = 0);
      virtual ~MeasureLeafItem();


    private: // Disallow copying of this class
      MeasureLeafItem(const MeasureLeafItem & other);
      const MeasureLeafItem & operator=(const MeasureLeafItem & other);
  };
}

#endif
