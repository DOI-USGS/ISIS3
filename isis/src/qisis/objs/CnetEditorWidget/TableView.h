#ifndef TableView_H
#define TableView_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QWidget>

#include "AbstractTableModel.h"

class QLabel;
template<typename T> class QList;
class QString;

namespace Isis {
  class AbstractTableModel;
  class AbstractTreeItem;
  class ControlPoint;
  class TableViewContent;
  class TableViewHeader;
  class TableColumnList;

  /**
    * @author ????-??-?? Unknown
    *
    * @internal
    *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
    *   @history 2017-05-18 Tracie Sucharski - Added a signal to indicate the control point chosen
    *                           from either the point table or the measure table.  If the point was
    *                           chosen from the measure table, the serial number of the measure is
    *                           also passed.  This was added for IPCE, for the interaction with
    *                           other views.
    *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
    *   @history 2017-08-08 Makayla Shepherd - Fixed a seg fault in ipce that occurs when
    *                           attempting to edit a control point when there is not an active
    *                           control network. Fixes #5048.
    */
  class TableView : public QWidget {
      Q_OBJECT

    public:
      TableView(AbstractTableModel *someModel,
          QString pathForSettigs, QString objName);
      virtual ~TableView();
      // QSize sizeHint() const;
      QFont getContentFont() const;
      TableViewHeader *getHorizontalHeader();

      QStringList getTitles() const;
      void setTitles(QStringList someTitle);

      void setColumnVisible(QString, bool);

      AbstractTableModel *getModel();
      //       void setModel(AbstractTableModel * newModel);
      void readSettings();
      void writeSettings();
      TableViewContent *content();


    public slots:
      void displayWarning(AbstractTableModel::Warning);
      void handleModelSelectionChanged();
      void handleModelSelectionChanged(QList< AbstractTreeItem * >);


    signals:
      void activated();
      void rebuildModels(QList< AbstractTreeItem * >);
      void selectionChanged();
      void modelDataChanged();
      void tableSelectionChanged(QList< AbstractTreeItem * >);
      void filterCountsChanged(int visibleRows, int totalRows);

      void editControlPoint(ControlPoint *, QString);

    private:
      // disable copying and assigning of this class
      TableView(const TableView &);
      TableView &operator=(const TableView &other);

      // methods
      void nullify();

      // data
      TableViewHeader *m_header;      //!< The table header
      TableViewContent *m_content;    //!< The content of the header
      TableColumnList *m_columns;     //!< The columns of the table
      AbstractTableModel *m_model;    //!< The model of the table
      QString *m_settingsPath;        //!< Path of where to read/write the settings
      QLabel *m_warningLabel;         //!< Label of any warnings
  };
}

#endif
