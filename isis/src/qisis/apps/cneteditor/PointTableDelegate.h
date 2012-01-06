#ifndef PointTableDelegate_H
#define PointTableDelegate_H

#include "AbstractTableDelegate.h"

class QString;
class QWidget;

namespace Isis
{
  namespace CnetViz
  {
    class AbstractTreeItem;

    /**
     * @brief Delegate for creating, reading, and saving data in the point table
     *
     * This class is responsible for creating widgets that can be used to edit
     * cells in the point table. It is also responsible for populating the
     * widgets with values and for saving the values.
     *
     * @author ????-??-?? Eric Hyer
     *
     * @internal
     */
    class PointTableDelegate : public AbstractTableDelegate
    {
      public:
        PointTableDelegate();
        virtual ~PointTableDelegate();

        QWidget * getWidget(TableColumn const *) const;

        void readData(QWidget *, AbstractTreeItem *,
                      TableColumn const *) const;
                      
        void readData(QWidget *, AbstractTreeItem *, TableColumn const *,
                      QString) const;
                      
        void saveData(QWidget *, AbstractTreeItem *,
                      TableColumn const *) const;


      private:
        PointTableDelegate(const PointTableDelegate &);
        PointTableDelegate & operator=(const PointTableDelegate &);

      private:
        static bool const warnOnSigmaEdit = true;
    };
  }
}

#endif

