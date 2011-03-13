#ifndef MeasureTableDelegate_H
#define MeasureTableDelegate_H


#include <QItemDelegate>


class QTableView;



namespace Isis
{
  class ControlMeasure;
  class MeasureTableModel;

  class MeasureTableDelegate : public QItemDelegate
  {
      Q_OBJECT

    public:
      MeasureTableDelegate(MeasureTableModel * tm, QTableView * tv,
          QObject * parent = 0);
      virtual ~MeasureTableDelegate();

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
      MeasureTableModel * tableModel;
      QTableView * tableView;
  };
}

#endif
