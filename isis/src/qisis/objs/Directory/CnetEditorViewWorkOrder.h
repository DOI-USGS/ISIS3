#ifndef CnetEditorViewWorkOrder_H
#define CnetEditorViewWorkOrder_H
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
class ControlList;
class Directory;
class Project;

  /**
   * @brief This work order allows the user to open a cnet editor (table) view of a single control network.
   * This workorder is synchronous and undoable.
   *
   * @author 2012-09-19 Steven Lambright
   *
   * @internal
   *   @history 2016-06-23 Tyler Wilson - Replaced QList<Control *> with ControlList *.
   *   @history 2017-04-16 J Bonn - Updated to new workorder design #4764.
   *   @history 2017-07-24 Cole Neuabuer - Set m_isSavedToHistory to false on construction
   *                           Fixes #4715
   *   @history 2017-07-25 Cole Neubauer - Added project()->setClean call #4969
   *   @history 2017-08-11 Cole Neubauer - Removed isUndoable and set parent member variable
   *                          Fixes #5064
   *   @history 2017-11-02  Tyler Wilson - Added a null pointer check on the ControList *controls
   *                          pointer in the isExecutable(...) function to prevent potential
   *                          segfaults.  References #4492.
   *   @history 2018-04-07 Tracie Sucharski - Clean up includes.
   */
  class CnetEditorViewWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      CnetEditorViewWorkOrder(Project *project);
      CnetEditorViewWorkOrder(const CnetEditorViewWorkOrder &other);
      ~CnetEditorViewWorkOrder();

      virtual CnetEditorViewWorkOrder *clone() const;

      virtual bool isExecutable(ControlList *controls);
      bool setupExecution();

    protected:
      bool dependsOn(WorkOrder *other) const;
      void execute();
      void undoExecution();

    private:
      CnetEditorViewWorkOrder &operator=(const CnetEditorViewWorkOrder &rhs);
  };
}
#endif
