#ifndef TableColumnList_H
#define TableColumnList_H

#include <QObject>

template<typename A, typename B> struct QPair;


namespace Isis {
  namespace CnetViz {
    class TableColumn;

    /**
     * @author ????-??-?? Unknown
     *
     * @internal
     *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
     *   @history 2016-08-28 Kelvin Rodriguez - Removed unused member functions that are would
     *                           cause infinite recursion if called. Caught by clang diagnostics.
     *                           Part of porting to OS X 10.11
     */
    class TableColumnList : public QObject {
        Q_OBJECT

      public:
        TableColumnList();
        TableColumnList(TableColumnList const &);
        virtual ~TableColumnList();

        TableColumn * &operator[](int index);
        TableColumn * &operator[](QString title);

        void append(TableColumn *newCol);
        void prepend(TableColumn *newCol);

        int indexOf(TableColumn const *someCol) const;
        bool contains(TableColumn const *someCol) const;
        bool contains(QString columnTitle) const;

        QPair< int, int > getVisibleXRange(int visibleColumn);
        TableColumnList getVisibleColumns();

        int getVisibleWidth() const;

        QList< TableColumn * > getSortingOrder();
        QStringList getSortingOrderAsStrings() const;
        void setSortingOrder(QStringList newOrder);
        void lower(TableColumn *col, bool emitSortOutDated = true);
        // void lower(int visibleColumnIndex, bool emitSortOutDated = true);
        void raise(TableColumn *col, bool emitSortOutDated = true);
        // void raise(int visibleColumnIndex, bool emitSortOutDated = true);
        void raiseToTop(TableColumn *col);
        // void raiseToTop(int visibleColumnIndex);

        int size() const;

        TableColumnList &operator=(TableColumnList other);


      signals:
        void sortOutDated();


      private:
        void checkIndexRange(int index);
        void nullify();


      private:
        QList< TableColumn * > * m_cols;
        QList< TableColumn * > * m_sortingOrder;
    };
  }
}

#endif
