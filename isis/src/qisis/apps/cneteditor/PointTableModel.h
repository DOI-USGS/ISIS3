#ifndef PointTableModel_H
#define PointTableModel_H


#include "AbstractTableModel.h"


class QStringList;
template< class T > class QList;


namespace Isis
{
  namespace CnetViz
  {
    class AbstractTreeItem;
    class AbstractTreeModel;

    class PointTableModel : public AbstractTableModel
    {
        Q_OBJECT

      public:
        explicit PointTableModel(AbstractTreeModel * model);
        virtual ~PointTableModel();

        virtual QList< AbstractTreeItem * > getItems(int, int);
        virtual QList< AbstractTreeItem * > getItems(AbstractTreeItem *,
            AbstractTreeItem *);
        virtual int getVisibleRowCount() const;
        virtual QList< AbstractTreeItem * > getSelectedItems();
        virtual QString getWarningMessage(AbstractTreeItem const *,
            TableColumn const *, QString valueToSave) const;
        virtual void setGlobalSelection(bool selected);
        virtual int indexOfVisibleItem(AbstractTreeItem const * item) const;

        static QString getPointWarningMessage(AbstractTreeItem const *,
            TableColumn const *, QString valueToSave);
        
        
      public slots:
        void handleTreeSelectionChanged(QList< AbstractTreeItem * >);


      protected:
        virtual TableColumnList * createColumns();
        
        
      private:
        PointTableModel(const PointTableModel &);
        PointTableModel & operator=(PointTableModel);
    };
  }
}

#endif
