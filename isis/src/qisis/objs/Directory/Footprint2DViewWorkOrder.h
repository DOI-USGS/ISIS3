#ifndef Footprint2DViewWorkOrder_H
#define Footprint2DViewWorkOrder_H
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
  class ImageList;
  class Project;
  class ShapeList;
  /**
   * @brief View an image list's footprints in a footprint view.
   *
   * Adding a Footprint2DView to the Project is not undo-able, so all functionality to add the view
   * is put into the execute method.  We want a WorkOrder rather than simply a QAction so that the
   * WorkOrder is added to the history.
   *
   * @todo Some thought should probably be given to whether we actually want view WorkOrders to be
   * saved in the history.
   *
   * @author 2012-09-19 Steven Lambright
   *
   * @internal
   *   @history 2016-01-08 Jeffrey Covington - Updated for new Footprint2DView, syncUndo()
   *                          was removed, and execute now uses WorkOrder's execute() method
   *   @history 2016-06-06 Makayla Shepherd - Updated documentation. Fixes #3954.
   *   @history 2016-06-27 Ian Humphrey - Updated documentation and coding standards for
   *                           AbstractProjectItemView, ControlMeasureEditWidget,
   *                           ControlPointEditWidget, CubeDnView, Footprint2DView.
   *                           Updated includes for CubeDnViewWorkOrder and
   *                           Footprint2DViewWorkOrder. Fixes #4004.
   *   @history 2016-10-21 Tracie Sucharski - Added support for ShapeLists.  Added back the
   *                           capability for choosing either a new view or using an existing view.
   *   @history 2016-02-06 Tracie Sucharski - Made this WorkOrder not undo-able by calling
   *                           setUndoRedo to false on parent class, and removing syncRedo and
   *                           syncUndo. The work is now done in the execute method. Fixes #4598.
   *   @history 2017-04-10 Tracie Sucharski - Refactor for the new WorkOrder design, renaming
   *                           execute to setupExecution, and moving the actual work to the execute
   *                           method.
   *   @history 2017-07-24 Cole Neuabuer - Set m_isSavedToHistory to false on construction
   *                           Fixes #4715
   *   @history 2017-07-25 Cole Neubauer - Added project()->setClean call #4969
   */
  class Footprint2DViewWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      Footprint2DViewWorkOrder(Project *project);
      Footprint2DViewWorkOrder(const Footprint2DViewWorkOrder &other);
      ~Footprint2DViewWorkOrder();

      virtual Footprint2DViewWorkOrder *clone() const;

      virtual bool isExecutable(ImageList *images);
      virtual bool isExecutable(ShapeList *shapes);
      bool isSavedToHistory() const;
      virtual bool setupExecution();
      virtual void execute();

    private:
      Footprint2DViewWorkOrder &operator=(const Footprint2DViewWorkOrder &rhs);
  };
}
#endif
