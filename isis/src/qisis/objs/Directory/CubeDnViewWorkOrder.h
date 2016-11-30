#ifndef CubeDnViewWorkOrder_H
#define CubeDnViewWorkOrder_H
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
  class ImageList;
  class Project;
  class ShapeList;

  /**
   * This work order is designed to bring up a qview-like view for a small number of cubes.
   *
   * @author 2012-09-19 Steven Lambright
   *
   * @internal
   *   @history 2016-01-13 Jeffrey Covington - Redesigned CNetSuiteMainWindow. Added CubeDNView
   *                          and Footprint2DView.
   *   @history 2016-06-06 Makayla Shepherd - Update documentation. Fixes #3993.
   *   @history 2016-07-27 Tracie Sucharski - Added support for shape models.
   *   @history 2016-09-09 Tracie Sucharski - Put option to choose either creating a new view or
   *                          adding images to an existing view.
   *   @history 2016-11-02 Makayla Shepherd - Changed the display text from View Raw Cubes to 
   *                          Display Images. Fixes #4494. 
   */
  class CubeDnViewWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      CubeDnViewWorkOrder(Project *project);
      CubeDnViewWorkOrder(const CubeDnViewWorkOrder &other);
      ~CubeDnViewWorkOrder();

      virtual CubeDnViewWorkOrder *clone() const;

      virtual bool isExecutable(ImageList *images);
      virtual bool isExecutable(ShapeList *shapes);

      bool execute();

    protected:
      bool dependsOn(WorkOrder *other) const;
      void syncRedo();

    private:
      CubeDnViewWorkOrder &operator=(const CubeDnViewWorkOrder &rhs);
  };
}
#endif

