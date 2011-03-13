#ifndef PointTableDelegate_H
#define PointTableDelegate_H


#include <QItemDelegate>

class QTableView;


namespace Isis
{
  class ControlMeasure;
  class PointTableModel;

  class PointTableDelegate : public QItemDelegate
  {
      Q_OBJECT

    public:
      PointTableDelegate(PointTableModel * tm, QTableView * tv,
          QObject * parent = 0);
      virtual ~PointTableDelegate();

      QWidget * createEditor(QWidget * parent,
          const QStyleOptionViewItem & option,
          const QModelIndex & index) const;

      void setEditorData(QWidget * editor, const QModelIndex & index) const;

      void setModelData(QWidget * editor, QAbstractItemModel * model,
          const QModelIndex & index) const;

      void updateEditorGeometry(QWidget * editor,
          const QStyleOptionViewItem & option, const QModelIndex & index) const;


    signals:
      void dataEdited() const;


    private:
      PointTableModel * tableModel;
      QTableView * tableView;
  };
}

#endif
