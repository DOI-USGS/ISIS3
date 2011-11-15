#ifndef MeasureTableModel_H
#define MeasureTableModel_H


#include "AbstractTableModel.h"


class QStringList;
template< class T > class QList;


namespace Isis
{
  namespace CnetViz
  {
    class AbstractTreeItem;
    class TableColumn;
    class TableColumnList;
    class AbstractTreeModel;
    
    class MeasureTableModel : public AbstractTableModel
    {
        Q_OBJECT

      public:
        explicit MeasureTableModel(AbstractTreeModel * model);
        virtual ~MeasureTableModel();

        virtual QList< AbstractTreeItem * > getItems(int, int);
        virtual QList< AbstractTreeItem * > getItems(AbstractTreeItem *,
            AbstractTreeItem *);
        virtual int getVisibleRowCount() const;
        virtual QList< AbstractTreeItem * > getSelectedItems();
        virtual QString getWarningMessage(AbstractTreeItem const *,
            TableColumn const *, QString valueToSave) const;
        virtual void setGlobalSelection(bool selected);
        virtual int indexOfVisibleItem(AbstractTreeItem const * item) const;

        static QString getMeasureWarningMessage(AbstractTreeItem const *,
            TableColumn const *, QString valueToSave);
        
        
      public slots:
        void handleTreeSelectionChanged(QList< AbstractTreeItem * >);
        

      protected:
        virtual TableColumnList * createColumns();
        
        
      private slots:
        void calculateFilterCounts();


      private:
        MeasureTableModel(MeasureTableModel const &);
        MeasureTableModel & operator=(MeasureTableModel const &);
    };
  }
}

#endif
