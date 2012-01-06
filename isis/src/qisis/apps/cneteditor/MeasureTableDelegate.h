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

    /**
     * Delegate for creating, reading, and saving data in the measure table.
     *
     * This class is responsible for creating widgets that can be used to edit
     * cells in the measure table. It is also responsible for populating the
     * widgets with values and for saving the values.
     *
     * @author ????-??-?? Eric Hyer
     *
     * @internal
     */
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

