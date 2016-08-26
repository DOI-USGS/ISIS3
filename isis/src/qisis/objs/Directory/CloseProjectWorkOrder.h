#ifndef CloseProjectWorkOrder_H
#define CloseProjectWorkOrder_H
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
   * This opens a project that's saved on disk.
   *
   * @author 2014-04-15 Ken Edmundson
   *
   * @internal
   *   @history 2016-06-06 Makayla Shepherd - Updated documentation. Fixes #3993.
   *   @history 2016-08-25 Adam Paqeutte - Updated documentation. Fixes #4299.
   */
  class CloseProjectWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      CloseProjectWorkOrder(Project *project);
      CloseProjectWorkOrder(const CloseProjectWorkOrder &other);
      ~CloseProjectWorkOrder();

      virtual CloseProjectWorkOrder *clone() const;

      virtual bool isExecutable();
      bool execute();

    signals:

    private:
      CloseProjectWorkOrder &operator=(const CloseProjectWorkOrder &rhs);
  };
}

#endif // CloseProjectWorkOrder_H
