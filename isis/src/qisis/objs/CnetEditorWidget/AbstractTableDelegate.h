#ifndef AbstractTableDelegate_H
#define AbstractTableDelegate_H

#include <QObject>

class QString;
class QWidget;

namespace Isis {
  class AbstractTreeItem;
  class TableColumn;

  /**
   * Base class for delegates which create, read, and save data in the tables.
   *
   * This class is responsible for the interface for creating widgets that
   * can be used to edit cells in the tables. This also provides an interface
   * for populating the widgets with values and for saving the values.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal 
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054. 
   */
  class AbstractTableDelegate : public QObject {
      Q_OBJECT

    public:
      AbstractTableDelegate();
      virtual ~AbstractTableDelegate();


      virtual QWidget *getWidget(TableColumn const *) const = 0;
      virtual void readData(QWidget *, AbstractTreeItem *,
          TableColumn const *) const = 0;
      virtual void readData(QWidget *, AbstractTreeItem *,
          TableColumn const *, QString) const = 0;
      virtual void saveData(QWidget *, AbstractTreeItem *,
          TableColumn const *) const = 0;


    private:
      AbstractTableDelegate(const AbstractTableDelegate &);
      AbstractTableDelegate &operator=(const AbstractTableDelegate &);
  };
}

#endif

