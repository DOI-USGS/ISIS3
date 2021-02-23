#ifndef ControlHealthMonitorWorkOrder_H
#define ControlHealthMonitorWorkOrder_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "WorkOrder.h"

namespace Isis {
  class Control;

  /**
   * @brief  This is a child of class WorkOrder which is used for anything that performs
   *  an action in a Project.  This work order allows the user to view the ControlHealthMonitor
   *  for the project's active Control Network.
   *
   * @author 2016-06-22 Adam Goins
   *
   * @internal
   * @history 2018-06-07 Adam Goins - Initial Version.
   */

  class ControlHealthMonitorWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      ControlHealthMonitorWorkOrder(Project *project);
      ControlHealthMonitorWorkOrder(const ControlHealthMonitorWorkOrder &other);
      ~ControlHealthMonitorWorkOrder();

      virtual ControlHealthMonitorWorkOrder *clone() const;

      virtual bool isExecutable(ControlList *controls);

      bool setupExecution();
      void execute();

    private:
      ControlHealthMonitorWorkOrder &operator=(const ControlHealthMonitorWorkOrder &rhs);
  };
}
#endif
