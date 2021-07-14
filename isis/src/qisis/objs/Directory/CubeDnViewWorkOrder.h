#ifndef CubeDnViewWorkOrder_H
#define CubeDnViewWorkOrder_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
   *   @history 2017-04-06 Makayla Shepherd - Added isUndoable, and renamed execute() to
   *                          setupExecution(), and syncRedo() to execute() according to the
   *                          WorkOrder redesign.
   *   @history 2017-07-24 Cole Neuabuer - Set m_isSavedToHistory to false on construction
   *                           Fixes #4715
   *   @history 2017-08-11 Cole Neubauer - Removed isUndoable and set parent member variable
   *                          Fixes #5064
   *   @history 2017-11-02 Tyler Wilson - Added null pointer checks for the ImageList *images
   *                          and ShapeList *shapes variables in their respective isExeuctable()
   *                          methods to prevent potential seg faults.  References #4492.
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

      bool isUndoable() const;

      bool setupExecution();
      void execute();

    protected:
      bool dependsOn(WorkOrder *other) const;

    private:
      CubeDnViewWorkOrder &operator=(const CubeDnViewWorkOrder &rhs);
  };
}
#endif
