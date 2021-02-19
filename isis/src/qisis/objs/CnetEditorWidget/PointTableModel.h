#ifndef PointTableModel_H
#define PointTableModel_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractTableModel.h"


class QStringList;
template< class T > class QList;


namespace Isis {
  class AbstractTreeItem;
  class AbstractTreeModel;

  /**
   * @brief Table model for control points
   *
   * This class represents a model that provides access to control points in a
   * table-like fashion. It acts as a proxy model to the underlying tree model
   * by providing an interface to get items by index ranges, get selected
   * items, etc.. Thus, users of the class can access all control points in
   * the underlying tree model without having to worry about the details of
   * traversing the tree structure to find the items of interest. The class
   * also has knowledge of what columns should be in the control point table.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class PointTableModel : public AbstractTableModel {
      Q_OBJECT

    public:
      explicit PointTableModel(AbstractTreeModel *model);
      virtual ~PointTableModel();

      virtual QList< AbstractTreeItem * > getItems(int, int);
      virtual QList< AbstractTreeItem * > getItems(AbstractTreeItem *,
          AbstractTreeItem *);
      virtual int getVisibleRowCount() const;
      virtual QList< AbstractTreeItem * > getSelectedItems();
      virtual QString getWarningMessage(AbstractTreeItem const *,
          TableColumn const *, QString valueToSave) const;
      virtual void setGlobalSelection(bool selected);
      virtual int indexOfVisibleItem(AbstractTreeItem const *item) const;

      static QString getPointWarningMessage(AbstractTreeItem const *,
          TableColumn const *, QString valueToSave);


    public slots:
      void handleTreeSelectionChanged(QList< AbstractTreeItem * >);


    protected:
      virtual TableColumnList *createColumns();


    private:
      PointTableModel(const PointTableModel &);
      PointTableModel &operator=(PointTableModel);
  };
}

#endif
