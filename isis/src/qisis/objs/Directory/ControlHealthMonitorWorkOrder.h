#ifndef ControlHealthMonitorWorkOrder_H
#define ControlHealthMonitorWorkOrder_H
/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
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
