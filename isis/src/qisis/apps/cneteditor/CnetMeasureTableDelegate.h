#ifndef CnetMeasureTableDelegate_H
#define CnetMeasureTableDelegate_H

#include "AbstractCnetTableDelegate.h"

class QString;
class QWidget;

namespace Isis
{
  class AbstractTreeItem;
  class CnetTableColumn;

  class CnetMeasureTableDelegate : public AbstractCnetTableDelegate
  {
    public:
      CnetMeasureTableDelegate();
      virtual ~CnetMeasureTableDelegate();

      QWidget * getWidget(CnetTableColumn const *) const;
      void readData(QWidget *, AbstractTreeItem *, CnetTableColumn const *)
          const;
      void saveData(QWidget *, AbstractTreeItem *, CnetTableColumn const *)
          const;


    private:
      CnetMeasureTableDelegate(const CnetMeasureTableDelegate &);
      CnetMeasureTableDelegate & operator=(const CnetMeasureTableDelegate &);
  };
}

#endif

