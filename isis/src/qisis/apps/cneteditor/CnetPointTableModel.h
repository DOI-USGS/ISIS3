#ifndef CnetPointTableModel_H
#define CnetPointTableModel_H


#include "AbstractCnetTableModel.h"


class QStringList;
template< class T > class QList;


namespace Isis
{
  class AbstractTreeItem;
  class TreeModel;

  class CnetPointTableModel : public AbstractCnetTableModel
  {
      Q_OBJECT

    public:
      explicit CnetPointTableModel(TreeModel * model);
      virtual ~CnetPointTableModel();

      virtual QList< AbstractTreeItem * > getItems(int, int);
      virtual QList< AbstractTreeItem * > getItems(AbstractTreeItem *,
          AbstractTreeItem *);
      virtual CnetTableColumnList createColumns();
      virtual int getVisibleRowCount() const;
      virtual QList< AbstractTreeItem * > getSelectedItems();
      virtual QString getWarningMessage(AbstractTreeItem const *,
          CnetTableColumn const *, QString valueToSave) const;
      virtual void setGlobalSelection(bool selected);


    private:
      CnetPointTableModel(const CnetPointTableModel &);
      CnetPointTableModel & operator=(CnetPointTableModel);
  };
}

#endif
