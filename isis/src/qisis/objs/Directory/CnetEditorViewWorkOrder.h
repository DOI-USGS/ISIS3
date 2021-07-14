#ifndef CnetEditorViewWorkOrder_H
#define CnetEditorViewWorkOrder_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "WorkOrder.h"

namespace Isis {
class ControlList;
class Directory;
class Project;

  /**
   * @brief This work order allows the user to open a cnet editor (table) view of a single control network.
   * This workorder is synchronous and undoable.
   *
   * @author 2012-09-19 Steven Lambright
   *
   * @internal
   *   @history 2016-06-23 Tyler Wilson - Replaced QList<Control *> with ControlList *.
   *   @history 2017-04-16 J Bonn - Updated to new workorder design #4764.
   *   @history 2017-07-24 Cole Neuabuer - Set m_isSavedToHistory to false on construction
   *                           Fixes #4715
   *   @history 2017-07-25 Cole Neubauer - Added project()->setClean call #4969
   *   @history 2017-08-11 Cole Neubauer - Removed isUndoable and set parent member variable
   *                          Fixes #5064
   *   @history 2017-11-02  Tyler Wilson - Added a null pointer check on the ControList *controls
   *                          pointer in the isExecutable(...) function to prevent potential
   *                          segfaults.  References #4492.
   *   @history 2018-04-07 Tracie Sucharski - Clean up includes.
   */
  class CnetEditorViewWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      CnetEditorViewWorkOrder(Project *project);
      CnetEditorViewWorkOrder(const CnetEditorViewWorkOrder &other);
      ~CnetEditorViewWorkOrder();

      virtual CnetEditorViewWorkOrder *clone() const;

      virtual bool isExecutable(ControlList *controls);
      bool setupExecution();

    protected:
      bool dependsOn(WorkOrder *other) const;
      void execute();
      void undoExecution();

    private:
      CnetEditorViewWorkOrder &operator=(const CnetEditorViewWorkOrder &rhs);
  };
}
#endif
