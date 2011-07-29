#include "IsisDebug.h"

#include "CnetTableView.h"

#include <QMargins>
#include <QVBoxLayout>

#include "AbstractCnetTableModel.h"
#include "CnetTableViewHeader.h"
#include "CnetTableViewContent.h"
#include "CnetTableColumn.h"
#include "CnetTableColumnList.h"


namespace Isis
{
  CnetTableView::CnetTableView()
  {
    nullify();

    active = false;

    columns = new CnetTableColumnList;

    content = new CnetTableViewContent(columns);
    header = new CnetTableViewHeader(columns);
    connect(content, SIGNAL(selectionChanged()),
        this, SIGNAL(selectionChanged()));
    connect(content, SIGNAL(horizontalScrollBarValueChanged(int)),
        header, SLOT(updateHeaderOffset(int)));
    connect(header, SIGNAL(columnResized(bool)),
        content, SLOT(updateHorizontalScrollBar(bool)));

    QVBoxLayout * layout = new QVBoxLayout;
    layout->addWidget(header);
    layout->addWidget(content);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setLayout(layout);
  }


  CnetTableView::~CnetTableView()
  {
    if (columns)
    {
      for (int i = 0; i < columns->size(); i++)
      {
        delete(*columns)[i];
        (*columns)[i] = NULL;
      }

      delete columns;
      columns = NULL;
    }
  }


  CnetTableViewHeader * CnetTableView::getHorizontalHeader()
  {
    return header;
  }


  QStringList CnetTableView::getTitles() const
  {
    // TODO: need a good title
    return QStringList();// header->getText();
  }


  void CnetTableView::setColumnVisible(QString column, bool visible)
  {
    foreach(CnetTableColumn * col, *columns)
    {
      if (col->getTitle() == column)
        col->setVisible(visible);
    }
  }


  AbstractCnetTableModel * CnetTableView::getModel()
  {
    return content->getModel();
  }


  void CnetTableView::setModel(AbstractCnetTableModel * newModel)
  {
    ASSERT(content);

    if (newModel)
    {
      ASSERT(header);
      connect(newModel, SIGNAL(filterProgressChanged(int)),
          header, SLOT(updateFilterProgress(int)));
      connect(newModel, SIGNAL(rebuildProgressChanged(int)),
          header, SLOT(updateRebuildProgress(int)));
      connect(newModel, SIGNAL(filterProgressRangeChanged(int, int)),
          header, SLOT(updateFilterProgressRange(int, int)));
      connect(newModel, SIGNAL(rebuildProgressRangeChanged(int, int)),
          header, SLOT(updateRebuildProgressRange(int, int)));
      connect(newModel, SIGNAL(filterCountsChanged(int, int)),
          header, SLOT(handleFilterCountsChanged(int, int)));
      connect(header, SIGNAL(requestedGlobalSelection(bool)),
          newModel, SLOT(setGlobalSelection(bool)));
      connect(header, SIGNAL(requestedGlobalSelection(bool)),
          this, SLOT(onModelSelectionChanged()));
      connect(header, SIGNAL(requestedGlobalSelection(bool)),
          this, SIGNAL(selectionChanged()));

      foreach(CnetTableColumn * col, *columns)
      {
        delete col;
        col = NULL;
      }

      *columns = newModel->createColumns();

      // Add a column for row numbers and global selection.
      columns->prepend(new CnetTableColumn("", true));

      foreach(CnetTableColumn * column, *columns)
      {
        column->setWidth(QFontMetrics(font()).width(column->getTitle()) + 25);
        connect(column, SIGNAL(visibilityChanged()), header, SLOT(update()));
        connect(column, SIGNAL(visibilityChanged()), content, SLOT(refresh()));
        connect(column, SIGNAL(visibilityChanged()),
            content, SLOT(updateHorizontalScrollBar()));
        connect(column, SIGNAL(widthChanged()), content, SLOT(refresh()));
      }

      content->setModel(newModel);
      header->update();
    }
  }


  void CnetTableView::onModelSelectionChanged()
  {
    content->refresh();
  }


  void CnetTableView::nullify()
  {
    header = NULL;
    content = NULL;
    columns = NULL;
  }
}

