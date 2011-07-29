#ifndef CnetMeasureTableDelegate_H
#define CnetMeasureTableDelegate_H

#include "AbstractCnetTableDelegate.h"

class QString;
class QWidget;

namespace Isis
{
  class AbstractTreeItem;

  class CnetMeasureTableDelegate : public AbstractCnetTableDelegate
  {
    public:
      CnetMeasureTableDelegate();
      virtual ~CnetMeasureTableDelegate();

      QWidget * getWidget(QString) const;
      void readData(QWidget *, AbstractTreeItem *, QString) const;
      void saveData(QWidget *, AbstractTreeItem *, QString) const;


    private:
      CnetMeasureTableDelegate(const CnetMeasureTableDelegate &);
      CnetMeasureTableDelegate & operator=(const CnetMeasureTableDelegate &);
  };
}

#endif

