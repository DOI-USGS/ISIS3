#ifndef AbstractTableDelegate_H
#define AbstractTableDelegate_H

#include <QObject>

class QString;
class QWidget;

namespace Isis
{
  namespace CnetViz
  {
    class AbstractTreeItem;
    class TableColumn;

    class AbstractTableDelegate : public QObject
    {
        Q_OBJECT

      public:
        AbstractTableDelegate();
        virtual ~AbstractTableDelegate();


        virtual QWidget * getWidget(TableColumn const *) const = 0;
        virtual void readData(QWidget *, AbstractTreeItem *,
            TableColumn const *) const = 0;
        virtual void readData(QWidget *, AbstractTreeItem *,
            TableColumn const *, QString) const = 0;
        virtual void saveData(QWidget *, AbstractTreeItem *,
            TableColumn const *) const = 0;


      private:
        AbstractTableDelegate(const AbstractTableDelegate &);
        AbstractTableDelegate & operator=(const AbstractTableDelegate &);
    };
  }
}

#endif

