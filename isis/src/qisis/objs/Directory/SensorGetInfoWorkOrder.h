#ifndef SensorGetInfoWorkOrder_H
#define SensorGetInfoWorkOrder_H
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
  class GuiCamera;

  /**
   * @brief This is a child of class WorkOrder which is used for anything that performs
   *  an action in a Project. This work order allows the user to view target body info.
   *  This runs synchronously and is currently not undoable.
   *
   * @author 2015-07-10 Ken Edmundson
   *
   * @internal
   *   @author 2016-06-06 Tyler Wilson - Added documentation for the functions and
   *              brought the code into compliance with ISIS coding standards.
   *              References #3944.
   *   @history 2017-04-16 J Bonn - Updated to new workorder design #4764.
   *   @history 2017-07-24 Cole Neuabuer - Set m_isSavedToHistory to false on construction
   *                           Fixes #4715
   */

  class SensorGetInfoWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      SensorGetInfoWorkOrder(Project *project);
      SensorGetInfoWorkOrder(const SensorGetInfoWorkOrder &other);
      ~SensorGetInfoWorkOrder();

      virtual SensorGetInfoWorkOrder *clone() const;

      virtual bool isExecutable(GuiCameraQsp camera);
      bool setupExecution();

    protected:
      bool dependsOn(WorkOrder *other) const;
      void execute();
      void undoExecution();

    private:
      SensorGetInfoWorkOrder &operator=(const SensorGetInfoWorkOrder &rhs);
  };
}
#endif
