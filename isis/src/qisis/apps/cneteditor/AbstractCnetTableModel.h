#ifndef AbstractCnetTableModel_H
#define AbstractCnetTableModel_H


#include <QObject>


template< class T > class QList;


namespace Isis
{
  class AbstractCnetTableDelegate;
  class AbstractTreeItem;
  class CnetTableColumn;
  class CnetTableColumnList;
  class TreeModel;

  class AbstractCnetTableModel : public QObject
  {
      Q_OBJECT

    public:
      AbstractCnetTableModel(TreeModel *, AbstractCnetTableDelegate *);
      virtual ~AbstractCnetTableModel();


      virtual QList< AbstractTreeItem * > getItems(int, int) = 0;
      virtual QList< AbstractTreeItem * > getItems(AbstractTreeItem *,
          AbstractTreeItem *) = 0;
      virtual QList< AbstractTreeItem * > getSelectedItems() = 0;
      virtual bool isFiltering();
      virtual CnetTableColumnList createColumns() = 0;
      virtual int getVisibleRowCount() const = 0;
      virtual QString getWarningMessage(AbstractTreeItem const *,
          CnetTableColumn const *, QString valueToSave) const = 0;
      virtual int indexOfVisibleItem(AbstractTreeItem const * item) const = 0;

      virtual const AbstractCnetTableDelegate * getDelegate() const;
      

    public slots:
      virtual void setGlobalSelection(bool selected) = 0;
      virtual void applyFilter();


    signals:
      void modelModified();
      void filterProgressChanged(int);
      void rebuildProgressChanged(int);
      void filterProgressRangeChanged(int, int);
      void rebuildProgressRangeChanged(int, int);
      void filterCountsChanged(int visibleTopLevelItemCount,
          int topLevelItemCount);
      void selectionChanged(QList<AbstractTreeItem *>);


    protected:
      TreeModel * getDataModel();
      const TreeModel * getDataModel() const;


    private:
      AbstractCnetTableModel(AbstractCnetTableModel const &);
      AbstractCnetTableModel & operator=(AbstractCnetTableModel const &);


    private:
      TreeModel * dataModel;
      AbstractCnetTableDelegate * delegate;
  };
}

#endif
