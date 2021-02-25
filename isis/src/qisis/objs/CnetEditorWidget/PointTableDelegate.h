#ifndef PointTableDelegate_H
#define PointTableDelegate_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractTableDelegate.h"

class QString;
class QWidget;

namespace Isis {
  class AbstractTreeItem;

  /**
   * @brief Delegate for creating, reading, and saving data in the point table
   *
   * This class is responsible for creating widgets that can be used to edit
   * cells in the point table. It is also responsible for populating the
   * widgets with values and for saving the values.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class PointTableDelegate : public AbstractTableDelegate {
    public:
      PointTableDelegate();
      virtual ~PointTableDelegate();

      QWidget *getWidget(TableColumn const *) const;

      void readData(QWidget *, AbstractTreeItem *,
          TableColumn const *) const;

      void readData(QWidget *, AbstractTreeItem *, TableColumn const *,
          QString) const;

      void saveData(QWidget *, AbstractTreeItem *,
          TableColumn const *) const;


    private:
      PointTableDelegate(const PointTableDelegate &);
      PointTableDelegate &operator=(const PointTableDelegate &);

    private:
      static bool const m_warnOnSigmaEdit = true;
  };
}

#endif
