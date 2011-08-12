#ifndef CnetTableColumnList_H
#define CnetTableColumnList_H
#include <SpecialPixel.h>


template<typename A, typename B> class QPair;


namespace Isis
{
  class CnetTableColumn;
  

  class CnetTableColumnList
  {
    public:
      CnetTableColumnList();
      CnetTableColumnList(CnetTableColumnList const &);
      virtual ~CnetTableColumnList();
      
      CnetTableColumn *& operator[](int index);
      
      void append(CnetTableColumn * newCol);
      void prepend(CnetTableColumn * newCol);
      
      int indexOf(CnetTableColumn const * someCol);
      bool contains(CnetTableColumn const * someCol);

      QPair< int, int > getVisibleXRange(int visibleColumn);
      CnetTableColumnList getVisibleColumns();
      
      int getVisibleWidth() const;
      
      QList< CnetTableColumn const * > getSortingOrder() const;
      void lower(CnetTableColumn const *);
      void lower(int visibleColumnIndex);
      void raise(CnetTableColumn const *);
      void raise(int visibleColumnIndex);
      
      int size() const;
      
      CnetTableColumnList & operator=(CnetTableColumnList other);
      
      
    private:
      void checkIndexRange(int index);
      void nullify();
      
      
    private:
      QList< CnetTableColumn * > * cols;
      QList< CnetTableColumn const * > * sortingOrder;
  };
}

#endif

