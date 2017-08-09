#ifndef SetActiveControlWorkOrder_H
#define SetActiveControlWorkOrder_H
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
   *  an action in a Project.  This work order allows the user to set the active ControlNet in the
   *  project.  Views that need to operate on a common control network, ie. footprint2dview,
   *  controlpointeditview, etc. can get the active control net from project.
   *
   * @author 2016-06-22 Tracie Sucharski
   *
   * @internal
   * @history 2016-09-19 Tracie Sucharski - Call SetImages on the control net to initialize cameras.
   * @history 2017-01-09 Tracie Sucharski - Moved the SetImages step to the
   *                         Project::setActiveControl.
   * @history 2017-01-30 Tracie Sucharski - Print active control in the Undo text.
   *          2017-04-04 Tracie Sucharski - Updated to reflect the new WorkOrder design, renaming
   *                         execute to setupExecution, and moving the actual work to execute.
   *                         Fixes #4717, #4728.
   * @history 2017-08-03 Cole Neubauer - Created a try catch around a previously unprotected error
   *                         to handle errors thrown in the workorder that halted execution.
   *                         Fixes #5026
   */

  class SetActiveControlWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      SetActiveControlWorkOrder(Project *project);
      SetActiveControlWorkOrder(const SetActiveControlWorkOrder &other);
      ~SetActiveControlWorkOrder();

      virtual SetActiveControlWorkOrder *clone() const;

      virtual bool isExecutable(ControlList *controls);

      bool setupExecution();
      void execute();

    private:
      SetActiveControlWorkOrder &operator=(const SetActiveControlWorkOrder &rhs);
  };
}
#endif
