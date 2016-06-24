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
   * @brief  This is a child of class WorkOrder which is used for anything that performs
   *  an action in a Project.  This work order displays a control network in 3D in
   *  an OpenGL view.
   *
   * @author 2014-04-04 Ken Edmundson
   *
   * @internal
   *   @author 2016-06-06 Tyler Wilson - Added documentation for the functions and
   *              brought the code into compliance with ISIS3 coding standards.
   *              References #3944.
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
      bool execute();

    protected:
      bool dependsOn(WorkOrder *other) const;
      void syncRedo();
      void syncUndo();

    private:
      ViewControlNet3DWorkOrder &operator=(const ViewControlNet3DWorkOrder &rhs);
  };
}
#endif

