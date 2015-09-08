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
   * This work order allows the user to view target body info.
   *
   * @author 2015-06-12 Ken Edmundson
   *
   * @internal
   */

  class TargetGetInfoWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      TargetGetInfoWorkOrder(Project *project);
      TargetGetInfoWorkOrder(const TargetGetInfoWorkOrder &other);
      ~TargetGetInfoWorkOrder();

      virtual TargetGetInfoWorkOrder *clone() const;

      virtual bool isExecutable(TargetBody *target);
      bool execute();

    protected:
      bool dependsOn(WorkOrder *other) const;
      void syncRedo();
      void syncUndo();

    private:
      TargetGetInfoWorkOrder &operator=(const TargetGetInfoWorkOrder &rhs);
  };
}
#endif

