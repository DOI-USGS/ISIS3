#ifndef RenameProjectWorkOrder_H
#define RenameProjectWorkOrder_H
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
   * Change the project's GUI name
   *
   * @author 2012-11-20 Steven Lambright
   *
   * @internal
   *   @history 2012-12-19 Steven Lambright and Stuart Sides - Added isNameValid() and changed
   *                           execute() to return false if the project name isn't changing, so that
   *                           the work order is thrown away.
   */
  class RenameProjectWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      RenameProjectWorkOrder(QString newName, Project *project);
      RenameProjectWorkOrder(Project *project);
      RenameProjectWorkOrder(const RenameProjectWorkOrder &other);
      ~RenameProjectWorkOrder();

      virtual RenameProjectWorkOrder *clone() const;

      bool isExecutable(Context context);

      bool execute();

      static bool isNameValid(QString nameToCheck);

    protected:
      bool dependsOn(WorkOrder *other) const;
      void syncRedo();
      void syncUndo();

    private:
      RenameProjectWorkOrder &operator=(const RenameProjectWorkOrder &rhs);
  };
}
#endif

