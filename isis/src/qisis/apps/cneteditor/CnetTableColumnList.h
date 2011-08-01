#ifndef CnetTableColumnList_H
#define CnetTableColumnList_H

#include <QList>

class QRect;

template<typename A, typename B> class QPair;

namespace Isis
{
  class CnetTableColumn;

  class CnetTableColumnList : public QList<CnetTableColumn *>
  {
    public:
      CnetTableColumnList();
      virtual ~CnetTableColumnList();

      QPair<int, int> getVisibleXRange(int visibleColumn)
      const;
      CnetTableColumnList getVisibleColumns();
      QList< const CnetTableColumn * > getVisibleColumns() const;
      int getVisibleWidth() const;
  };
}

#endif

