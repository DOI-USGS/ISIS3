#include "CnetTableColumnList.h"

#include <QPair>

#include "CnetTableColumn.h"

namespace Isis
{
  CnetTableColumnList::CnetTableColumnList()
  {
  }


  CnetTableColumnList::~CnetTableColumnList()
  {
  }


  /**
   * @returns minX, maxX in a pair
   */
  QPair<double, double> CnetTableColumnList::getVisibleXRange(
    int visibleColumn) const
  {
    double minX = 0;
    double maxX = 0;

    QList<const CnetTableColumn *> visibleCols = getVisibleColumns();

    if (visibleColumn < visibleCols.size() && visibleColumn >= 0)
    {
      int indent = 0;

      for (int i = 0; i < visibleColumn; i++)
        indent += visibleCols.at(i)->getWidth() - 1;

      minX = indent;
      maxX = minX + visibleCols.at(visibleColumn)->getWidth() - 1;
    }

    return QPair<double, double>(minX, maxX);
  }


  CnetTableColumnList CnetTableColumnList::getVisibleColumns()
  {
    CnetTableColumnList visibleColumns;

    for (int i = 0; i < size(); i++)
      if (at(i)->isVisible())
        visibleColumns.append(at(i));

    return visibleColumns;
  }


  QList<const CnetTableColumn *>
  CnetTableColumnList::getVisibleColumns() const
  {
    QList<const CnetTableColumn *> visibleColumns;

    for (int i = 0; i < size(); i++)
      if (at(i)->isVisible())
        visibleColumns.append(at(i));

    return visibleColumns;
  }


  int CnetTableColumnList::getVisibleWidth() const
  {
    int width = 0;

    for (int i = 0; i < size(); i++)
      if (at(i)->isVisible())
        width += at(i)->getWidth() - 1;
    // For the border...
    width -= 2;

    return width;
  }
}

