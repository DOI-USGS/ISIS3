/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "TableView.h"

#include <iostream>

#include <QLabel>
#include <QMargins>
#include <QSettings>
#include <QVBoxLayout>

#include "AbstractTableModel.h"
#include "IException.h"
#include "TableViewHeader.h"
#include "TableViewContent.h"
#include "TableColumn.h"
#include "TableColumnList.h"


namespace Isis {
  /**
    * Constructor
    *
    * @param someModel The abstract table model to view
    * @param pathForSettings The path to read/write settings to
    * @param objName The name of the object
    */
  TableView::TableView(AbstractTableModel *someModel,
      QString pathForSettings,
      QString objName) {
    nullify();

    m_settingsPath = new QString(pathForSettings);
    setObjectName(objName);

    m_warningLabel = new QLabel;

    m_model = someModel;
    connect(m_model, SIGNAL(filterCountsChanged(int, int)),
        this, SIGNAL(filterCountsChanged(int, int)));
    connect(m_model, SIGNAL(userWarning(AbstractTableModel::Warning)),
        this, SLOT(displayWarning(AbstractTableModel::Warning)));

    m_columns = m_model->getColumns();

    // Add a column for row numbers and global selection.
    m_columns->prepend(new TableColumn("", true, false));

    QSettings settings(*m_settingsPath, QSettings::NativeFormat);
    QString key;
    for (int i = 0; i < m_columns->size(); i++) {
      TableColumn *const &col = (*m_columns)[i];
      QString colTitle = col->getTitle();
      int defaultWidth = QFontMetrics(font()).width(colTitle) + 40;
      if (colTitle.size()) {
        key = objectName() + " " + colTitle + " width";
        key.replace(" ", "_");
        col->setWidth(settings.value(key, defaultWidth).toInt());

        key = objectName() + " " + colTitle + " ascending";
        key.replace(" ", "_");
        col->setSortAscending(settings.value(key, true).toBool());
      }
      else {
        col->setWidth(defaultWidth);

        // no need to set sort order since it is already ascending by default
      }
    }

    key = objectName() + " sorting order";
    key.replace(" ", "_");
    try {
      m_columns->setSortingOrder(
        settings.value(key, QStringList()).toStringList());
    }
    catch (IException &) {
    }

    m_header = new TableViewHeader(m_model);
    connect(m_header, SIGNAL(requestedGlobalSelection(bool)),
        this, SLOT(handleModelSelectionChanged()));
    connect(m_header, SIGNAL(requestedGlobalSelection(bool)),
        this, SIGNAL(selectionChanged()));

    m_content = new TableViewContent(m_model);
    connect(m_content, SIGNAL(tableSelectionChanged()),
        this, SIGNAL(selectionChanged()));
    connect(m_content,
        SIGNAL(rebuildModels(QList< AbstractTreeItem * >)),
        this,
        SIGNAL(rebuildModels(QList< AbstractTreeItem * >)));
    connect(m_content, SIGNAL(horizontalScrollBarValueChanged(int)),
        m_header, SLOT(updateHeaderOffset(int)));
    connect(m_content, SIGNAL(modelDataChanged()),
        this, SIGNAL(modelDataChanged()));
    connect(m_content, SIGNAL(tableSelectionChanged(QList<AbstractTreeItem *>)),
        this, SIGNAL(tableSelectionChanged(QList<AbstractTreeItem *>)));
    connect(m_content, SIGNAL(editControlPoint(ControlPoint *, QString)),
            this, SIGNAL(editControlPoint(ControlPoint *, QString)));

    connect(m_header, SIGNAL(columnResized(bool)),
        m_content, SLOT(updateHorizontalScrollBar(bool)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_header);
    layout->addWidget(m_content);
    layout->addWidget(m_warningLabel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setLayout(layout);
  }


  /**
    * Destructor
    */
  TableView::~TableView() {
    // save column widths
    if (m_settingsPath->size() && objectName().size()) {
      QSettings settings(*m_settingsPath, QSettings::NativeFormat);
      QString key;
      for (int i = 0; i < m_columns->size(); i++) {
        TableColumn *const &col = (*m_columns)[i];
        QString colTitle = col->getTitle();
        if (colTitle.size()) {
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
      settings.setValue(key, m_columns->getSortingOrderAsStrings());
    }

    delete m_model;
    m_model = NULL;

    m_columns = NULL;
  }


  /**
    * Returns the horizontal header
    *
    * @return TableViewHeader The horizontal header
    */
  TableViewHeader *TableView::getHorizontalHeader() {
    return m_header;
  }


  /**
    * Sets the specified column visible or invisible
    *
    * @param column The column to change the visibility of
    * @param visible The visibility setting
    */
  void TableView::setColumnVisible(QString column, bool visible) {
    for (int i = 0; i < m_columns->size(); i++) {
      TableColumn *col = (*m_columns)[i];
      if (col->getTitle() == column)
        col->setVisible(visible);
    }
  }


  /**
    * Returns the model
    *
    * @return AbstractTableModel The model
    */
  AbstractTableModel *TableView::getModel() {
    return m_content->getModel();
  }


  /**
    * Returns the content of the table
    *
    * @return TableViewContent The content of the table
    */
  TableViewContent *TableView::content() {
    return m_content;
  }


  //   void TableView::setModel(AbstractTableModel * newModel)
  //   {
  //
  //     if (newModel)
  //     {
  //       connect(newModel, SIGNAL(filterProgressChanged(int)),
  //           m_header, SLOT(updateFilterProgress(int)));
  //       connect(newModel, SIGNAL(rebuildProgressChanged(int)),
  //           m_header, SLOT(updateRebuildProgress(int)));
  //       connect(newModel, SIGNAL(filterProgressRangeChanged(int, int)),
  //           m_header, SLOT(updateFilterProgressRange(int, int)));
  //       connect(newModel, SIGNAL(rebuildProgressRangeChanged(int, int)),
  //           m_header, SLOT(updateRebuildProgressRange(int, int)));
  //       connect(newModel, SIGNAL(filterCountsChanged(int, int)),
  //           m_header, SLOT(handleFilterCountsChanged(int, int)));
  //       connect(m_header, SIGNAL(requestedGlobalSelection(bool)),
  //           newModel, SLOT(setGlobalSelection(bool)));
  //       connect(m_header, SIGNAL(requestedGlobalSelection(bool)),
  //           this, SLOT(onModelSelectionChanged()));
  //       connect(m_header, SIGNAL(requestedGlobalSelection(bool)),
  //           this, SIGNAL(selectionChanged()));
  //
  //       m_columns = newModel->getColumns();
  //
  //       // Add a column for row numbers and global selection.
  //       m_columns->prepend(new TableColumn("", true, false));
  //
  //       for (int i = 0; i < m_columns->size(); i++)
  //       {
  //         TableColumn * column = (*m_columns)[i];
  //
  //         column->setWidth(QFontMetrics(font()).width(column->getTitle()) + 25);
  //         connect(column, SIGNAL(visibilityChanged()), m_header, SLOT(update()));
  //         connect(column, SIGNAL(visibilityChanged()), m_content, SLOT(refresh()));
  //         connect(column, SIGNAL(visibilityChanged()),
  //             m_content, SLOT(updateHorizontalScrollBar()));
  //         connect(column, SIGNAL(widthChanged()), m_content, SLOT(refresh()));
  //       }
  //
  //       m_header->setColumns(m_columns);
  //       m_content->setModel(newModel);
  //       m_header->update();
  //     }
  //   }


  /**
    * Displays warnings for a table
    *
    * @param warning The waarning that will be displayed
    */
  void TableView::displayWarning(AbstractTableModel::Warning warning) {
    switch (warning) {
      case AbstractTableModel::SortingDisabled:
        m_warningLabel->setText(tr("<font color='red'>Sorting disabled</font>"));
        m_warningLabel->setVisible(true);
        break;

      case AbstractTableModel::SortingTableSizeLimitReached:
        m_warningLabel->setText(
          tr("<font color='red'>Sorting disabled - table row count (%L1) > table size limit"
              " (%L2)</font>")
          .arg(m_content->getModel()->getVisibleRowCount())
          .arg(m_content->getModel()->sortLimit()));
        m_warningLabel->setVisible(true);
        break;

      case AbstractTableModel::None:
        m_warningLabel->setText(tr(""));
        m_warningLabel->setVisible(false);
        break;
    }
  }

  /**
    * Handles refreshing the content when the model selection is changed.
    */
  void TableView::handleModelSelectionChanged() {
    m_content->refresh();
  }


  void TableView::handleModelSelectionChanged(
    QList< AbstractTreeItem * > newlySelectedItems) {
    m_content->refresh();
    m_content->scrollTo(newlySelectedItems);
  }

  /**
    * Sets all member variables to NULL
    */
  void TableView::nullify() {
    m_header = NULL;
    m_content = NULL;
    m_columns = NULL;
    m_model = NULL;
    m_settingsPath = NULL;
  }
}
