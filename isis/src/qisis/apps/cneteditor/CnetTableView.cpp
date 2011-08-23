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
#include <QSettings>


using std::cerr;


namespace Isis
{
  CnetTableView::CnetTableView(AbstractCnetTableModel * someModel,
                               QString pathForSettings,
                               QString objName)
  {
    nullify();
    
    settingsPath = new QString(pathForSettings);
    setObjectName(objName);
    
    model = someModel;
    
    columns = model->getColumns();
    
    // Add a column for row numbers and global selection.
    columns->prepend(new CnetTableColumn("", true, false));
    
    QSettings settings(*settingsPath, QSettings::NativeFormat);
    QString key;
    for (int i = 0; i < columns->size(); i++)
    {
      CnetTableColumn * const & col = (*columns)[i];
      QString colTitle = col->getTitle();
      int defaultWidth = QFontMetrics(font()).width(colTitle) + 40;
      if (colTitle.size())
      {
        key = objectName() + " " + colTitle + " width";
        key.replace(" ", "_");
        col->setWidth(settings.value(key, defaultWidth).toInt());
        
        key = objectName() + " " + colTitle + " ascending";
        key.replace(" ", "_");
        col->setSortAscending(settings.value(key, true).toBool());
      }
      else
      {
        col->setWidth(defaultWidth);
        
        // no need to set sort order since it is already ascending by default
      }
    }
    
    key = objectName() + " sorting order";
    key.replace(" ", "_");
    columns->setSortingOrder(settings.value(key, QStringList()).toStringList());
    
    header = new CnetTableViewHeader(model);
    connect(header, SIGNAL(requestedGlobalSelection(bool)),
            this, SLOT(handleModelSelectionChanged()));
    connect(header, SIGNAL(requestedGlobalSelection(bool)),
            this, SIGNAL(selectionChanged()));
    
    content = new CnetTableViewContent(model);
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
    // save column widths
    ASSERT(objectName().size());
    if (settingsPath->size() && objectName().size())
    {
      QSettings settings(*settingsPath, QSettings::NativeFormat);
      QString key;
      for (int i = 0; i < columns->size(); i++)
      {
        CnetTableColumn * const & col = (*columns)[i];
        QString colTitle = col->getTitle();
        if (colTitle.size())
        {
          key = objectName() + " " + colTitle + " width";
          key.replace(" ", "_");
          settings.setValue(key, col->getWidth());
          
          key = objectName() + " " + colTitle + " ascending";
          key.replace(" ", "_");
          settings.setValue(key, col->sortAscending());
        }
      }
      
      key = objectName() + " sorting order";
      key.replace(" ", "_");
      settings.setValue(key, columns->getSortingOrderAsStrings());
    }
    
    delete model;
    model = NULL;
    
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


  void CnetTableView::handleModelSelectionChanged()
  {
    content->refresh();
  }


  void CnetTableView::handleModelSelectionChanged(
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
    model = NULL;
    settingsPath = NULL;
  }
}

