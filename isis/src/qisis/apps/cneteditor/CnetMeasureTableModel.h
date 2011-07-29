#ifndef CnetMeasureTableModel_H
#define CnetMeasureTableModel_H


#include "AbstractCnetTableModel.h"


class QStringList;
template< class T > class QList;


namespace Isis
{
  class AbstractTreeItem;
  class TreeModel;

  class CnetMeasureTableModel : public AbstractCnetTableModel
  {
      Q_OBJECT

    public:
      explicit CnetMeasureTableModel(TreeModel * model);
      virtual ~CnetMeasureTableModel();

      virtual QList< AbstractTreeItem * > getItems(int, int);
      virtual QList< AbstractTreeItem * > getItems(AbstractTreeItem *,
          AbstractTreeItem *);
      virtual CnetTableColumnList createColumns();
      virtual int getVisibleRowCount() const;
      virtual QList< AbstractTreeItem * > getSelectedItems();
      virtual void setGlobalSelection(bool selected);


    private slots:
      void calculateFilterCounts();


    private:
      CnetMeasureTableModel(const CnetMeasureTableModel &);
      CnetMeasureTableModel & operator=(CnetMeasureTableModel);
  };
}

#endif
