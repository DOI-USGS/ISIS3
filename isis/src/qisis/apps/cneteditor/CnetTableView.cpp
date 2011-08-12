#include "IsisDebug.h"

#include "CnetTableView.h"

#include <iostream>

#include <QMargins>
#include <QVBoxLayout>

#include "AbstractCnetTableModel.h"
#include "CnetTableViewHeader.h"
#include "CnetTableViewContent.h"
#include "CnetTableColumn.h"
#include "CnetTableColumnList.h"


namespace Isis
{
  CnetTableView::CnetTableView(AbstractCnetTableModel * someModel)
  {
    nullify();
    
    columns = someModel->getColumns();

    // Add a column for row numbers and global selection.
    columns->prepend(new CnetTableColumn("", true, false));
    
    for (int i = 0; i < columns->size(); i++)
    {
      (*columns)[i]->setWidth(
          QFontMetrics(font()).width((*columns)[i]->getTitle()) + 25);
    }
    
    
    header = new CnetTableViewHeader(someModel);
    connect(header, SIGNAL(requestedGlobalSelection(bool)),
            this, SLOT(onModelSelectionChanged()));
    connect(header, SIGNAL(requestedGlobalSelection(bool)),
            this, SIGNAL(selectionChanged()));
//     header->update();
    
    content = new CnetTableViewContent(someModel);
    connect(content, SIGNAL(tableSelectionChanged()),
            this, SIGNAL(selectionChanged()));
    connect(content, SIGNAL(rebuildModels(QList<AbstractTreeItem *>)),
            this, SIGNAL(rebuildModels(QList<AbstractTreeItem *>)));
    connect(content, SIGNAL(horizontalScrollBarValueChanged(int)),
            header, SLOT(updateHeaderOffset(int)));
    connect(content, SIGNAL(modelDataChanged()),
            this, SIGNAL(modelDataChanged()));
    connect(content, SIGNAL(tableSelectionChanged(QList<AbstractTreeItem*>)),
            this, SIGNAL(tableSelectionChanged(QList<AbstractTreeItem*>)));

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
    columns = NULL;
  }


  CnetTableViewHeader * CnetTableView::getHorizontalHeader()
  {
    return header;
  }


  void CnetTableView::setColumnVisible(QString column, bool visible)
  {
    for (int i = 0; i < columns->size(); i++)
    {
      CnetTableColumn * col = (*columns)[i];
      if (col->getTitle() == column)
        col->setVisible(visible);
    }
  }


  AbstractCnetTableModel * CnetTableView::getModel()
  {
    return content->getModel();
  }


//   void CnetTableView::setModel(AbstractCnetTableModel * newModel)
//   {
//     ASSERT(content);
// 
//     if (newModel)
//     {
//       ASSERT(header);
//       connect(newModel, SIGNAL(filterProgressChanged(int)),
//           header, SLOT(updateFilterProgress(int)));
//       connect(newModel, SIGNAL(rebuildProgressChanged(int)),
//           header, SLOT(updateRebuildProgress(int)));
//       connect(newModel, SIGNAL(filterProgressRangeChanged(int, int)),
//           header, SLOT(updateFilterProgressRange(int, int)));
//       connect(newModel, SIGNAL(rebuildProgressRangeChanged(int, int)),
//           header, SLOT(updateRebuildProgressRange(int, int)));
//       connect(newModel, SIGNAL(filterCountsChanged(int, int)),
//           header, SLOT(handleFilterCountsChanged(int, int)));
//       connect(header, SIGNAL(requestedGlobalSelection(bool)),
//           newModel, SLOT(setGlobalSelection(bool)));
//       connect(header, SIGNAL(requestedGlobalSelection(bool)),
//           this, SLOT(onModelSelectionChanged()));
//       connect(header, SIGNAL(requestedGlobalSelection(bool)),
//           this, SIGNAL(selectionChanged()));
// 
//       columns = newModel->getColumns();
// 
//       // Add a column for row numbers and global selection.
//       columns->prepend(new CnetTableColumn("", true, false));
// 
//       for (int i = 0; i < columns->size(); i++)
//       {
//         CnetTableColumn * column = (*columns)[i];
//         
//         column->setWidth(QFontMetrics(font()).width(column->getTitle()) + 25);
//         connect(column, SIGNAL(visibilityChanged()), header, SLOT(update()));
//         connect(column, SIGNAL(visibilityChanged()), content, SLOT(refresh()));
//         connect(column, SIGNAL(visibilityChanged()),
//             content, SLOT(updateHorizontalScrollBar()));
//         connect(column, SIGNAL(widthChanged()), content, SLOT(refresh()));
//       }
// 
//       header->setColumns(columns);
//       content->setModel(newModel);
//       header->update();
//     }
//   }


  void CnetTableView::onModelSelectionChanged()
  {
    content->refresh();
  }


  void CnetTableView::onModelSelectionChanged(
      QList< AbstractTreeItem * > newlySelectedItems)
  {
    content->refresh();
    content->scrollTo(newlySelectedItems);
  }


  void CnetTableView::nullify()
  {
    header = NULL;
    content = NULL;
    columns = NULL;
  }
}

