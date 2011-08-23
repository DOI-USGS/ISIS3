#ifndef CnetTableColumnList_H
#define CnetTableColumnList_H
#include <SpecialPixel.h>


template<typename A, typename B> class QPair;


namespace Isis
{
  class CnetTableColumn;
  

  class CnetTableColumnList : public QObject
  {
      Q_OBJECT
      
    public:
      CnetTableColumnList();
      CnetTableColumnList(CnetTableColumnList const &);
      virtual ~CnetTableColumnList();
      
      CnetTableColumn *& operator[](int index);
      CnetTableColumn *& operator[](QString title);
      
      void append(CnetTableColumn * newCol);
      void prepend(CnetTableColumn * newCol);
      
      int indexOf(CnetTableColumn const * someCol);
      bool contains(CnetTableColumn const * someCol);

      QPair< int, int > getVisibleXRange(int visibleColumn);
      CnetTableColumnList getVisibleColumns();
      
      int getVisibleWidth() const;
      
      QList< CnetTableColumn * > getSortingOrder();
      QStringList getSortingOrderAsStrings() const;
      void setSortingOrder(QStringList newOrder);
      void lower(CnetTableColumn * col, bool emitSortOutDated = true);
      void lower(int visibleColumnIndex, bool emitSortOutDated = true);
      void raise(CnetTableColumn * col, bool emitSortOutDated = true);
      void raise(int visibleColumnIndex, bool emitSortOutDated = true);
      void raiseToTop(CnetTableColumn * col);
      void raiseToTop(int visibleColumnIndex);
      
      int size() const;
      
      CnetTableColumnList & operator=(CnetTableColumnList other);
      
      
    signals:
      void sortOutDated();
      
      
    private:
      void checkIndexRange(int index);
      void nullify();
      
      
    private:
      QList< CnetTableColumn * > * cols;
      QList< CnetTableColumn * > * sortingOrder;
  };
}

#endif

