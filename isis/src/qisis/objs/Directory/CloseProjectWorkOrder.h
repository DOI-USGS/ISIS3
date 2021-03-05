#ifndef CloseProjectWorkOrder_H
#define CloseProjectWorkOrder_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "WorkOrder.h"

namespace Isis {

  /**
   * This opens a project that's saved on disk.
   *
   * @author 2014-04-15 Ken Edmundson
   *
   * @internal
   *   @history 2016-06-06 Makayla Shepherd - Updated documentation. Fixes #3993.
   *   @history 2016-08-25 Adam Paqeutte - Updated documentation. Fixes #4299.
   *   @history 2017-01-20 Tracie Sucharski - Add UndoCommand text to prevent error message.
   *                          Fixes #2146.
   *   @history 2017-07-18 Cole Neubauer - Finished implementing Close Project work order.
   *                          Fixes #4521
   *   @history 2017-07-18 Cole Neubauer - Updated if statment to check if something is open
   *                          Fixes #4960
   *   @history 2017-07-24 Cole Neuabuer - Set m_isSavedToHistory to false on construction
   *                           Fixes #4715
   *   @history 2017-08-11 Cole Neubauer - set isUndoable parent member variable
   *                          Fixes #5064
   */
  class CloseProjectWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      CloseProjectWorkOrder(Project *project);
      CloseProjectWorkOrder(const CloseProjectWorkOrder &other);
      ~CloseProjectWorkOrder();

      virtual CloseProjectWorkOrder *clone() const;

      virtual bool isExecutable();
      bool setupExecution();
      void execute();


    signals:

    private:
      CloseProjectWorkOrder &operator=(const CloseProjectWorkOrder &rhs);
  };
}

#endif // CloseProjectWorkOrder_H
