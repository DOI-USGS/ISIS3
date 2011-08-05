#ifndef AbstractCnetTableDelegate_H
#define AbstractCnetTableDelegate_H

#include <QObject>

class QString;
class QWidget;

namespace Isis
{
  class AbstractTreeItem;
  class CnetTableColumn;

  class AbstractCnetTableDelegate : public QObject
  {
      Q_OBJECT

    public:
      AbstractCnetTableDelegate();
      virtual ~AbstractCnetTableDelegate();


      virtual QWidget * getWidget(CnetTableColumn const *) const = 0;
      virtual void readData(QWidget *, AbstractTreeItem *,
          CnetTableColumn const *) const = 0;
      virtual void readData(QWidget *, AbstractTreeItem *,
          CnetTableColumn const *, QString) const = 0;
      virtual void saveData(QWidget *, AbstractTreeItem *,
          CnetTableColumn const *) const = 0;


    private:
      AbstractCnetTableDelegate(const AbstractCnetTableDelegate &);
      AbstractCnetTableDelegate & operator=(const AbstractCnetTableDelegate &);
  };
}

#endif

