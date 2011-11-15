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

