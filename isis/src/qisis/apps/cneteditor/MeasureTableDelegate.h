#ifndef MeasureTableDelegate_H
#define MeasureTableDelegate_H

#include "AbstractTableDelegate.h"

class QString;
class QWidget;

namespace Isis
{
  namespace CnetViz
  {
    class AbstractTreeItem;
    class TableColumn;

    class MeasureTableDelegate : public AbstractTableDelegate
    {
      public:
        MeasureTableDelegate();
        virtual ~MeasureTableDelegate();

        QWidget * getWidget(TableColumn const *) const;
        
        void readData(QWidget *, AbstractTreeItem *,
                      TableColumn const *) const;
                      
        void readData(QWidget *, AbstractTreeItem *, TableColumn const *,
                      QString) const;
                      
        void saveData(QWidget *, AbstractTreeItem *,
                      TableColumn const *) const;


      private:
        MeasureTableDelegate(const MeasureTableDelegate &);
        MeasureTableDelegate & operator=(const MeasureTableDelegate &);
    };
  }
}

#endif

