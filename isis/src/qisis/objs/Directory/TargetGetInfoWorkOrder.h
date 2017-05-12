#ifndef TargetGetInfoWorkOrder_H
#define TargetGetInfoWorkOrder_H
/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
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
  class TargetBody;

  /**
   * @brief This work order allows the user to view target body info.
   *
   * This is a child of class WorkOrder which is used for anything that performs
   * an action in a Project.  This work order allows the user to view target body info.
   *
   * @author 2015-06-12 Ken Edmundson
   *
   * @internal
   *   @history 2016-06-06 Tyler Wilson - Added documentation for the functions and
   *                           brought the code into compliance with ISIS3 coding standards.
   *                           References #3944.
   *   @history 2017-04-11 Ian Humphrey - Separated setup and execution of the work order into
   *                           setupExecution() and execute(). Removed syncRedo() and syncUndo().
   *                           Set m_isUndoable to false as work order is not undoable.
   *                           Fixes #4737.
   */
  class TargetGetInfoWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      TargetGetInfoWorkOrder(Project *project);
      TargetGetInfoWorkOrder(const TargetGetInfoWorkOrder &other);
      ~TargetGetInfoWorkOrder();

      virtual TargetGetInfoWorkOrder *clone() const;

      virtual bool isExecutable(TargetBodyQsp targetBody);
      virtual bool setupExecution();
      virtual void execute();

    protected:
      bool dependsOn(WorkOrder *other) const;

    private:
      TargetGetInfoWorkOrder &operator=(const TargetGetInfoWorkOrder &rhs);
  };
}
#endif
