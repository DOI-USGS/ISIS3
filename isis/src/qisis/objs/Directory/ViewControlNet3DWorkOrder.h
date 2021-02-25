#ifndef ViewControlNet3DWorkOrder_H
#define ViewControlNet3DWorkOrder_H
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
  /**
   * @brief  This work order displays a control network in 3D in an OpenGL view.
   * This runs synchronously and is undoable.
   * This is non-functioning and mostly unimplemented.
   *
   * @author 2014-04-04 Ken Edmundson
   *
   * @internal
   *   @author 2016-06-06 Tyler Wilson - Added documentation for the functions and
   *              brought the code into compliance with ISIS coding standards.
   *              References #3944.
   *   @history 2017-04-16 J Bonn - Updated to new workorder design #4764.
   *   @history 2017-07-24 Cole Neuabuer - Set m_isSavedToHistory to false on construction
   *                           Fixes #4715
   *   @history 2017-07-25 Cole Neubauer - Added project()->setClean call #4969
   *   @history 2017-11-02 Tyler Wilson - Added a null pointer check on the controls pointer
   *                           in the isExecutable function to prevent potential seg faults.
   *                           References #4492.
   */
  class ViewControlNet3DWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      ViewControlNet3DWorkOrder(Project *project);
      ViewControlNet3DWorkOrder(const ViewControlNet3DWorkOrder &other);
      ~ViewControlNet3DWorkOrder();

      virtual ViewControlNet3DWorkOrder *clone() const;

      //virtual bool isExecutable(QList<Control *> controls);
      virtual bool isExecutable(ControlList *controls);
      bool setupExecution();

    protected:
      bool dependsOn(WorkOrder *other) const;
      void execute();
      void undoExecution();

    private:
      ViewControlNet3DWorkOrder &operator=(const ViewControlNet3DWorkOrder &rhs);
  };
}
#endif
